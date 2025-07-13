#include "../head/dataStruct.h"

Tire node_list[NODE_NUM];	// 节点列表
ID_conversion ID_list[ID_LIST_SIZE]; // ID转换列表
int list_size = 0;
// Function to parse a hexadecimal character to integer
int hex_to_int(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return -1;
}

void transferIp(char* originIP, uint8_t* transIP) {
	int i = 0;
	int j = 0;
	int k = 0;
	char temp[4];
	int len = strlen(originIP);
	//ip一个最大为255，所以只需要3位
	for (i = 0; i <= len; i++) {
		temp[j++] = originIP[i];
		if (originIP[i] == '.' || originIP[i] == '\0') {
			temp[j - 1] = '\0';
			transIP[k++] = (uint8_t)atoi(temp);
			j = 0;
		}
	}
}

void transferIp6(const char* ipv6_str, uint16_t* result) {
	// Initialize variables
	uint16_t segments[8] = { 0 }; // To store the eight 16-bit segments
	int segment_index = 0;      // Current segment index
	int double_colon_index = -1;// Index of '::' if present
	int len = strlen(ipv6_str); // Length of the input string

	// Iterate over the string and parse segments
	for (int i = 0; i < len; i++) {
		if (ipv6_str[i] == ':') {
			// Check for double colon "::"
			if (i > 0 && ipv6_str[i - 1] == ':') {
				double_colon_index = segment_index;
				continue;
			}
			else if (i == 0 || ipv6_str[i - 1] == ':') {
				continue; // Skip single colons
			}
			segment_index++;
		}
		else {
			// Parse the hexadecimal value
			int value = hex_to_int(ipv6_str[i]);
			if (value == -1) continue; // Skip invalid characters

			segments[segment_index] = (segments[segment_index] << 4) | value;
		}
	}

	// Handle the case with "::" (zero compression)
	if (double_colon_index != -1) {
		int zero_fill_count = 8 - segment_index - 1;
		memmove(&segments[double_colon_index + zero_fill_count],
			&segments[double_colon_index],
			(segment_index - double_colon_index + 1) * sizeof(uint16_t));
		memset(&segments[double_colon_index], 0, zero_fill_count * sizeof(uint16_t));
	}

	// Copy segments to result
	for (int i = 0; i < 8; i++) {
		result[i] = segments[i];
	}
}

void initIDMap() {
	for (int i = 0; i < ID_LIST_SIZE; i++) {
		ID_list[i].client_ID = 0;
		ID_list[i].expire_time = 0;
		ID_list[i].msg = NULL;
		ID_list[i].msg_size = 0;

	}
}

int get_index(char c) {
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	else if (c >= 'a' && c <= 'z') {
		return c - 'a' + 10;
	}
	else if (c >= 'A' && c <= 'Z') {
		return c - 'A' + 10;
	}
	else if (c == '-') {
		return 36;
	}
	else if (c == '.') {
		return 37;
	}
	else {
		return -1;
	}
}

void initIps(Ips* ip) {
	ip->type = 0;
	ip->isFilledIp4 = 0;
	ip->isFilledIp6 = 0;
	ip->next = NULL;
	for (int i = 0; i < 4; i++) {
		ip->ip[i] = 0;
	}
	for (int i = 0; i < 8; i++) {
		ip->ip6[i] = 0;
	}
}

//tire树的相关操作
void initTire(Tire* root, int len) {
	for (int i = 0; i < len; i++) {
		root[i].isEnd = 0;
		for (int j = 0; j < 38; j++) {
			root[i].next[j] = 0;
		}
		root[i].pre = -1;
		root[i].ips = NULL;
	}
}

void insert4(char* domain, uint8_t* ip) {
	int len = strlen(domain);
	int i = 0;
	int num = 0;
	for (; i < len; i++) {
		int index = get_index(domain[i]);
		if (node_list[num].next[index] == 0) {
			node_list[num].next[index] = ++list_size;
			node_list[list_size].pre = i;
		}
		num = node_list[num].next[index];
	}
	//添加到Ips链表，头插
	Ips* temp = (struct Ips*)malloc(sizeof(struct Ips));
	temp->type = RR_A;
	for (int i = 0; i < 4; i++) {
		temp->ip[i] = ip[i];
	}
	temp->isFilledIp4 = 1;
	temp->isFilledIp6 = 0;
	temp->next = node_list[num].ips->next;
	node_list[num].ips->next = temp;
	node_list[num].isEnd = 1;
}

void insert6(char* domain, uint16_t* ip6) {
	int len = strlen(domain);
	int i = 0;
	int num = 0;
	for (; i < len; i++) {
		int index = get_index(domain[i]);
		if (node_list[num].next[index] == 0) {
			node_list[num].next[index] = ++list_size;
			node_list[list_size].pre = i;
		}
		num = node_list[num].next[index];
	}
	//添加到Ips链表，头插
	Ips* temp = (struct Ips*)malloc(sizeof(struct Ips));
	temp->type = RR_AAAA;
	for (int i = 0; i < 8; i++) {
		temp->ip6[i] = ip6[i];
	}
	temp->isFilledIp4 = 0;
	temp->isFilledIp6 = 1;
	temp->next = node_list[num].ips->next;
	node_list[num].ips->next = temp;
	node_list[num].isEnd = 1;
}

int search(char* domain, struct Ips** head) {
	(*head)->next = NULL;
	int findCount = 0;
	int len = strlen(domain);
	int num = 0;
	//搜索到就将其拷贝到head中
	for (int i = 0; i < len; i++) {
		int index = get_index(domain[i]);
		if (node_list[num].next[index] == 0) {
			return 0;
		}
		num = node_list[num].next[index];
	}
	if (node_list[num].isEnd == 0) {
		return 0;
	}
	else {
		Tire* temp_node = &node_list[num];
		Ips* temp = temp_node->ips->next;
		//比对temp_node中的ip地址type，将其拷贝到temp中
		for (; temp != NULL; temp = temp->next) {
			if (temp->type == (*head)->type) {
				struct Ips* p = (struct Ips*)malloc(sizeof(struct Ips));
				memcpy(p, temp, sizeof(struct Ips));
				p->next = (*head)->next;
				(*head)->next = p;
				findCount++;
			}
		}

		if (findCount > 0) {
			update_cache(domain, *head);
		}
	}
	return findCount;
}

void update_cache(char* domain, struct Ips* head) {
	lru_node* newNode = malloc(sizeof(struct node));

	if (cache_size >= MAX_CACHE) {
		delete_cache();
	}

	cache_size++;

	memcpy(newNode->domain, domain, strlen(domain) + 1);
	newNode->type = head->next->type;
	newNode->ips = malloc(sizeof(struct Ips));
	memcpy(newNode->ips, head->next, sizeof(struct Ips));


	newNode->next = lru_head->next;
	lru_head->next = newNode;
}

int search_cache(char* domain, struct Ips** head) {
	lru_node* ptr = lru_head;
	int cnt = 0;

	/* 先查找缓存中是否已存在域名，若存在则将其放到头部 */
	while (ptr && ptr->next) {
		if (strcmp(ptr->next->domain, domain) == 0 && ptr->next->type == (*head)->type) {

			struct Ips* temp = malloc(sizeof(struct Ips));
			memcpy(temp, ptr->next->ips, sizeof(struct Ips));
			struct Ips* p = temp;
			while (p->next) {
				p = p->next;
				cnt++;
			}
			p->next = (*head)->next;
			(*head)->next = temp;
			cnt++;

			/* 将该节点放到头部 */
			lru_node* tar = ptr->next;
			ptr->next = tar->next;

			tar->next = lru_head->next;
			lru_head->next = tar;
		}
		ptr = ptr->next;
	}

	return cnt;
}

void delete_cache() {
	lru_node* p = lru_head;
	while (p->next) {
		if (p->next->next == NULL) {
			lru_tail = p->next;
			p->next = NULL;
			free(lru_tail);
			cache_size--;
			return;
		}
		p = p->next;
	}
}
