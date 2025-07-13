#include "dataStruct.h"

trie_node node_list[NODE_NUM];		// 节点列表
ID_conversion ID_list[ID_LIST_SIZE]; // ID转换列表
size_t list_size = 0;

// Function to parse a hexadecimal character to integer
int hex_to_int(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';

	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;

	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;

	return -1;
}

void ipv4_to_bytes(const char* ipv4_str, uint8_t* ipv4_bytes)
{
	int i = 0;
	int j = 0;
	int k = 0;
	char temp[4];	//IP一个最大为255，所以只需要3 + 1 = 4位
	size_t len = strlen(ipv4_str);

	for (i = 0; i <= len; i++) {
		temp[j++] = ipv4_str[i];
		if (ipv4_str[i] == '.' || ipv4_str[i] == '\0') {
			temp[j - 1] = '\0';
			ipv4_bytes[k++] = (uint8_t)atoi(temp);
			j = 0;
		}
	}
}

void ipv6_to_bytes(const char* ipv6_str, uint16_t* ipv6_bytes)
{
	// Initialize variables
	uint16_t segments[8] = { 0 };	// To store the eight 16-bit segments
	int segment_index = 0;			// Current segment index
	int double_colon_index = -1;	// Index of '::' if present
	size_t len = strlen(ipv6_str);	// Length of the input string

	// Iterate over the string and parse segments
	for (int i = 0; i < len; i++) {
		if (ipv6_str[i] == ':') {
			// Check for double colon "::"
			if (i > 0 && ipv6_str[i - 1] == ':') {
				double_colon_index = segment_index;
				continue;
			}
			else if (i == 0 || ipv6_str[i - 1] == ':')
				continue; // Skip single colons
			segment_index++;
		}
		else {
			// Parse the hexadecimal value
			int result = hex_to_int(ipv6_str[i]);
			if (result == -1)
				continue; // Skip invalid characters
			segments[segment_index] = (segments[segment_index] << 4) | result;
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
	for (int i = 0; i < 8; i++)
		ipv6_bytes[i] = segments[i];
}

void init_id_map()
{
	for (int i = 0; i < ID_LIST_SIZE; i++) {
		ID_list[i].client_ID = 0;
		ID_list[i].expire_time = 0;
		ID_list[i].msg = NULL;
		ID_list[i].msg_size = 0;
	}
}

// 将一个域名中可能出现的所有字符映射到整型，用作下标
int get_index(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0'; // 0 ~ 9

	if (c >= 'a' && c <= 'z')
		return c - 'a' + 10; // 10 ~ 35

	if (c >= 'A' && c <= 'Z')
		return c - 'A' + 10; // 10 ~ 35，不区分大小写

	if (c == '-')
		return 36; // 36

	if (c == '.')
		return 37; // 37

	return -1;
}

void init_ip_list(ip_list head)
{
	head->version = 0;
	head->next = NULL;
	for (int i = 0; i < 4; i++)
		head->addr.ipv4[i] = 0;
	for (int i = 0; i < 8; i++)
		head->addr.ipv6[i] = 0;
}

//Trie树的相关操作
void init_trie(trie_node* root, int len)
{
	for (int i = 0; i < len; i++) {
		root[i].is_end = 0;
		for (int j = 0; j < 38; j++)
			root[i].next[j] = 0;
		root[i].ip_list = NULL;
	}
}

void insert4(char* domain, uint8_t* ip)
{
	int num = 0;
	for (int i = 0; domain[i]; i++) {
		int index = get_index(domain[i]);
		if (node_list[num].next[index] == 0)
			node_list[num].next[index] = ++list_size;
		num = node_list[num].next[index];
	}

	//添加到IP链表，头插
	ip_list_node* new_node = (ip_list_node*)malloc(sizeof(ip_list_node));
	new_node->version = 4;
	for (int i = 0; i < 4; i++)
		new_node->addr.ipv4[i] = ip[i];
	new_node->next = node_list[num].ip_list->next;
	node_list[num].ip_list->next = new_node;
	node_list[num].is_end = 1;
}

void insert6(char* domain, uint16_t* ip)
{
	int num = 0;
	for (int i = 0; domain[i]; i++) {
		int index = get_index(domain[i]);
		if (node_list[num].next[index] == 0)
			node_list[num].next[index] = ++list_size;
		num = node_list[num].next[index];
	}

	//添加到IP链表，头插
	ip_list_node* temp = (ip_list_node*)malloc(sizeof(ip_list_node));
	temp->version = 6;
	for (int i = 0; i < 8; i++)
		temp->addr.ipv6[i] = ip[i];
	temp->next = node_list[num].ip_list->next;
	node_list[num].ip_list->next = temp;
	node_list[num].is_end = 1;
}

int search(char* domain, ip_list head)
{
	head->next = NULL;
	int find_count = 0;
	size_t len = strlen(domain);
	int num = 0;

	//搜索到就将其拷贝到head中
	for (int i = 0; i < len; i++) {
		int index = get_index(domain[i]);
		if (node_list[num].next[index] == 0)
			return 0;
		num = node_list[num].next[index];
	}

	if (node_list[num].is_end == 0)
		return 0;
	else {
		trie_node* temp_node = &node_list[num];
		ip_list_node* temp = temp_node->ip_list->next;

		//比对temp_node中的ip地址version，将其拷贝到temp中
		for (; temp != NULL; temp = temp->next) {
			if (temp->version == head->version) {
				ip_list_node* p = (ip_list_node*)malloc(sizeof(ip_list_node));
				memcpy(p, temp, sizeof(ip_list_node));
				p->next = head->next;
				head->next = p;
				find_count++;
			}
		}
		if (find_count > 0)
			update_cache(domain, head);
	}
	return find_count;
}

void init_cache()
{
	/*
				 +------+     +------+
		head---->|      |---->|      |---->NULL
			     | head |     | tail |
		NULL<----|      |<----|      |<----tail
				 +------+     +------+
	*/

	lru_head = malloc(sizeof(lru_node));
	lru_tail = malloc(sizeof(lru_node));

	lru_head->prev = NULL;
	lru_head->next = lru_tail;
	lru_head->ip_list = NULL;

	lru_tail->prev = lru_head;
	lru_tail->next = NULL;
	lru_tail->ip_list = NULL;

	cache_size = 0;
}

void update_cache(char* domain, ip_list head)
{
	/*
		+------+     +----------+     +-----+
		|      |---->|          |---->|     |
		| head |     | new_node |     | ... |
		|      |<----|          |<----|     |
		+------+     +----------+     +-----+
	*/

	lru_node* new_node = malloc(sizeof(lru_node));

	if (cache_size >= MAX_CACHE)
		delete_cache();

	cache_size++;

	memcpy(new_node->domain, domain, strlen(domain) + 1);
	new_node->version = head->next->version;
	new_node->ip_list = malloc(sizeof(ip_list_node));
	memcpy(new_node->ip_list, head->next, sizeof(ip_list_node));

	new_node->prev = lru_head;
	new_node->next = lru_head->next;
	lru_head->next->prev = new_node;
	lru_head->next = new_node;
}

int search_cache(char* domain, ip_list head)
{
	lru_node* cur = lru_head;
	int cnt = 0;

	/* 查找缓存中是否已存在域名，若存在则将其放到头部 */
	while (cur->next) {
		if (strcmp(cur->next->domain, domain) == 0 && cur->next->version == head->version) {
			/*
				+------+     +-----+     +-----+     +-----+     +-----+
				|      |---->|     |---->|     |---->|     |---->|     |
				| head |     | ... |     | cur |     | tar |     | ... |
				|      |<----|     |<----|     |<----|     |<----|     |
				+------+     +-----+     +-----+     +-----+     +-----+
			*/
			ip_list_node* temp = malloc(sizeof(ip_list_node));
			memcpy(temp, cur->next->ip_list, sizeof(ip_list_node));
			ip_list_node* p = temp;
			while (p->next) {
				p = p->next;
				cnt++;
			}
			p->next = head->next;
			head->next = temp;
			cnt++;

			/* 将该节点放到头部 */
			lru_node* tar = cur->next;
			cur->next = tar->next;
			tar->next->prev = cur;

			tar->prev = lru_head;
			tar->next = lru_head->next;
			lru_head->next = tar;
			tar->next->prev = tar;
			break;
		}
		cur = cur->next;
	}

	return cnt;
}

void delete_cache()
{
	/*
		+-----+     +--------+     +------+
		|     |---->|        |---->|      |
		| ... |     | target |     | tail |
		|     |<----|        |<----|      |
		+-----+     +--------+     +------+
	*/

	lru_node* target = lru_tail->prev;
	lru_tail->prev = target->prev;
	lru_tail->prev->next = lru_tail;

	free(target->ip_list);
	free(target);
	cache_size--;
}
