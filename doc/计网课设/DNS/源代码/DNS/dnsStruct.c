#include "dnsStruct.h"

/* *​
* @brief 从DNS缓冲区中读取指定位数的数据并转换为合适的主机字节序
*
*  该函数从给定的DNS缓冲区指针中读取8位、16位或32位数据，并根据读取的位数
* 自动进行网络字节序到主机字节序的转换。读取后，缓冲区指针会向前移动相应的字节数。
*
* @param buffer 指向DNS缓冲区的指针的指针。函数会修改这个指针使其指向下一个未读取的数据。
* @param num 要读取的位数，必须是8、16或32中的一个。
*
* @return 返回读取并转换后的数据，以size_t类型返回。
* 对于8位数据直接返回原值，16位和32位数据会进行字节序转换。
*
* @note 调用者需要确保 :
* 1. buffer指向有效的内存区域且有足够的剩余数据可读
* 2. num参数只能是8、16或32中的一个
* 3. buffer指针在调用后会被修改，指向下一个未读取的数据
*
*@warning 如果传入的num不是8、16或32，函数行为是未定义的。
*/
size_t get_bits(dns_addr* buffer, int num)
{
	/* ntohs：网络字节顺序转换为主机字节顺序 */
	if (num == 8) {
		uint8_t val;
		memcpy(&val, *buffer, 1);
		*buffer += 1;
		return val;
	}
	if (num == 16) {
		uint16_t val;
		memcpy(&val, *buffer, 2);
		*buffer += 2;
		return ntohs(val);
	}
	if (num == 32) {
		uint32_t val;
		memcpy(&val, *buffer, 4);
		*buffer += 4;
		return ntohl(val);
	}
}

void set_bits(dns_addr* buffer, int len, int value)
{
	/* ntohs：网络字节顺序转换为主机字节顺序 */
	if (len == 8) {
		uint8_t val = value;
		memcpy(*buffer, &val, 1);
		*buffer += 1;
	}
	if (len == 16) {
		uint16_t val = htons(value);
		memcpy(*buffer, &val, 2);
		*buffer += 2;
	}
	if (len == 32) {
		uint32_t val = htonl(value);
		memcpy(*buffer, &val, 4);
		*buffer += 4;
	}
}

dns_addr get_header(struct dns_message* msg, dns_addr buffer, dns_addr start)
{
	msg->header->id = get_bits(&buffer, 16);
	uint16_t val = get_bits(&buffer, 16);
	msg->header->qr = (val & QR_MASK) >> 15;
	msg->header->opcode = (val & OPCODE_MASK) >> 11;
	msg->header->aa = (val & AA_MASK) >> 10;
	msg->header->tc = (val & TC_MASK) >> 9;
	msg->header->rd = (val & RD_MASK) >> 8;
	msg->header->ra = (val & RA_MASK) >> 7;
	msg->header->rcode = (val & RCODE_MASK) >> 0;
	msg->header->ques_num = get_bits(&buffer, 16);
	msg->header->ans_num = get_bits(&buffer, 16);
	msg->header->auth_num = get_bits(&buffer, 16);
	msg->header->add_num = get_bits(&buffer, 16);
	return buffer;
}

dns_addr get_question(struct dns_message* msg, dns_addr buffer, dns_addr start)
{
	int num;
	for (num = 0; num < msg->header->ques_num; num++) {
		char name[MAX_SIZE] = { 0 };
		struct dns_question* p = malloc(sizeof(struct dns_question));

		buffer = get_domain_name(buffer, name, start);
		//buffer向后移动
		p->q_name = (char*)malloc(strlen(name) + 1);
		memcpy(p->q_name, name, strlen(name) + 1);
		p->q_type = get_bits(&buffer, 16);
		p->q_class = get_bits(&buffer, 16);
		p->next = msg->question;
		msg->question = p;
	}
	return buffer;
}

dns_addr get_answer(struct dns_message* msg, dns_addr buffer, dns_addr start)
{
	for (int i = 0; i < msg->header->ans_num; i++) {
		char name[MAX_SIZE] = { 0 };
		struct dns_resource_record* p = malloc(sizeof(struct dns_resource_record));
		p->next = NULL;
		buffer = get_domain_name(buffer, name, start);

		p->name = malloc(strlen(name) + 1);
		memcpy(p->name, name, strlen(name) + 1);

		p->type = get_bits(&buffer, 16);
		p->class = get_bits(&buffer, 16);
		p->ttl = get_bits(&buffer, 32);
		p->data_len = get_bits(&buffer, 16);

		/* 获取IPv4地址 */
		if (p->type == RR_A) {
			for (int j = 0; j < 4; j++) {
				p->r_data.a_record.IP_addr[j] = *buffer;
				buffer++;
			}
		}
		/* 获取IPv6地址 */
		else if (p->type == RR_AAAA) {
			for (int j = 0; j < 8; j++) {
				uint16_t word = ((uint16_t)*buffer << 8) | *(buffer + 1);
				p->r_data.aaaa_record.IP_addr[j] = word;
				buffer += 2;
			}
		}
		else {
			buffer += p->data_len;
		}
		p->next = msg->answer;
		msg->answer = p;
	}

	return buffer;
}

int is_offset(uint16_t word)
{
	return (word & 0xc000) == 0xc000;
}
// 11
dns_addr get_domain_name(dns_addr buffer, char* name, dns_addr start)
{
	uint8_t* ptr = buffer;
	int i = 0;
	
	uint16_t word = ((uint16_t)*ptr << 8) | *(ptr + 1);
	if (is_offset(word)) { //前两比特为11
		uint16_t offset = word & 0x3fff;
		get_domain_name(start + offset, name, start);
		return buffer + 2;
	}

	int len = 0;

	while (1) {
		uint8_t val = *ptr;
		ptr++;
		if (val == 0 || (val & 0xc0) == 0xc0)
			return ptr;
		else if (len == 0) {
			len = val;
			if (i != 0) {
				name[i] = '.';
				i++;
			}
		}
		else if (len != 0) {
			name[i] = val;
			i++;
			len--;
		}
	}

	word = ((uint16_t)*ptr << 8) | *(ptr + 1);
	if (is_offset(word)) {
		char name2[MAX_SIZE] = { 0 };
		uint16_t offset = word & 0x3fff;
		get_domain_name(start + offset, name2, start);
		for (int j = 0; j < strlen(name2); j++)
			name[i + j] = name2[j];
		ptr += 2;
	}
	else if (*ptr == 0)
		ptr++;
	return ptr;
}

uint8_t* set_domain_name(dns_addr buffer, char* name)
{
	uint8_t* ptr = name;
	char tmp[MAX_SIZE] = { 0 };
	int i = 0;

	while (1) {
		if (*ptr == 0) {
			*buffer = i;
			buffer++;
			memcpy(buffer, tmp, i);
			buffer += i;

			*buffer = 0;
			buffer++;
			break;
		}
		else if (*ptr != '.')
			tmp[i++] = *ptr;
		else if (*ptr == '.') {
			*buffer = i;
			buffer++;
			memcpy(buffer, tmp, i);
			buffer += i;
			memset(tmp, 0, sizeof(tmp));
			i = 0;
		}
		ptr++;
	}

	return buffer;
}

dns_addr get_dns_message(struct dns_message* msg, dns_addr buffer, dns_addr start)
{
	msg->header = (struct dns_header*)malloc(sizeof(struct dns_header));
	msg->question = (struct dns_question*)malloc(sizeof(struct dns_question));
	msg->answer = NULL;

	buffer = get_header(msg, buffer, start);

	buffer = get_question(msg, buffer, start);

	buffer = get_answer(msg, buffer, start);

	return buffer;
}

dns_addr set_dns_message(struct dns_message* msg, dns_addr buffer, struct ip_list_node** head, int cnt)
{
	/* 组装报头 */
	buffer = set_header(msg, buffer, head, cnt);

	/* 组装询问 */
	buffer = set_question(msg, buffer);

	/* 组装回答 */
	buffer = set_answer(msg, buffer, head, cnt);

	return buffer;
}

dns_addr set_header(struct dns_message* msg, dns_addr buffer, struct ip_list_node** head, int cnt)
{
	int shield = 0;
	dns_header* header = msg->header;
	header->qr = 1;     // QR=1 应答报文
	header->aa = 0;     // 权威域名服务器
	header->ra = 1;     // 可用递归
	header->ans_num = cnt;

	if (head) {
		if ((*head)->next->version == 4) {
			ip_list_node* p = (*head)->next;
			while (p) {
				if (p->addr.ipv4[0] == 0 && p->addr.ipv4[1] == 0 && p->addr.ipv4[2] == 0 && p->addr.ipv4[3] == 0) {
					shield = 1;
					break;
				}
				p = p->next;
			}
		}

		if ((*head)->next->version == 6) {
			ip_list_node* p = (*head)->next;
			while (p) {
				if (p->addr.ipv6[0] == 0 && p->addr.ipv6[1] == 0 && p->addr.ipv6[2] == 0 && p->addr.ipv6[3] == 0 && p->addr.ipv6[4] == 0 && p->addr.ipv6[5] == 0 && p->addr.ipv6[6] == 0 && p->addr.ipv6[7] == 0) {
					shield = 1;
					break;
				}
				p = p->next;
			}
		}
	}

	if (!head)
		header->rcode = 2;  // 服务器错误
	else if (shield)
		/* 若查到0.0.0.0，则该域名被屏蔽 */
		header->rcode = 3;  // 名字错误
	else
		header->rcode = 0;  // 无差错

	set_bits(&buffer, 16, header->id);  // 设置ID

	int flags = 0;
	flags |= (header->qr << 15) & QR_MASK;
	flags |= (header->opcode << 11) & OPCODE_MASK;
	flags |= (header->aa << 10) & AA_MASK;
	flags |= (header->tc << 9) & TC_MASK;
	flags |= (header->rd << 8) & RD_MASK;
	flags |= (header->ra << 7) & RA_MASK;
	flags |= (header->rcode << 0) & RCODE_MASK;

	set_bits(&buffer, 16, flags);
	set_bits(&buffer, 16, header->ques_num);
	set_bits(&buffer, 16, header->ans_num);
	set_bits(&buffer, 16, header->auth_num);
	set_bits(&buffer, 16, header->add_num);

	return buffer;
}

dns_addr set_question(struct dns_message* msg, dns_addr buffer)
{
	int i;
	for (i = 0; i < msg->header->ques_num; i++) {
		struct dns_question* p = msg->question;
		buffer = set_domain_name(buffer, p->q_name);

		set_bits(&buffer, 16, p->q_type);
		set_bits(&buffer, 16, p->q_class);

		p = p->next;
	}
	return buffer;
}

dns_addr set_answer(dns_message* msg, dns_addr buffer, ip_list_node** head, int cnt)
{
	if (!head)
		return buffer;

	struct ip_list_node* p = (*head)->next;

	while (p) {
		set_bits(&buffer, 16, 0xc00c);  // name
		if (p->version == 4) // type
			set_bits(&buffer, 16, 1);
		else
			set_bits(&buffer, 16, 28);
		set_bits(&buffer, 16, 1);    // rr_class
		set_bits(&buffer, 32, 60);    // ttl
		if (p->version == 4) {      // rd_length
			set_bits(&buffer, 16, 4);
			for (int j = 0; j < 4; j++)
				set_bits(&buffer, 8, (p->addr.ipv4[j]));
		}
		else {
			set_bits(&buffer, 16, 16);
			for (int j = 0; j < 8; j++)
				set_bits(&buffer, 16, (p->addr.ipv6[j]));
		}
		p = p->next;
	}

	return buffer;
}

uint16_t add_list_id(uint16_t client_id, struct sockaddr_in client_addr, dns_message* msg, int msg_size)
{
	uint16_t i;
	for (i = 0; i < ID_LIST_SIZE; i++) {
		if (ID_list[i].expire_time < time(NULL)) {
			ID_list[i].client_ID = client_id;
			ID_list[i].client_addr = client_addr;
			ID_list[i].msg = (struct dns_message*)malloc(sizeof(struct dns_message));
			memcpy(ID_list[i].msg, msg, msg_size);
			ID_list[i].msg_size = msg_size;
			ID_list[i].expire_time = time(NULL) + ID_EXPIRE_TIME;
			break;
		}
	}
	if (i == ID_LIST_SIZE) {
		printf("ID list is full.\n");
		return ID_LIST_SIZE;
	}
	return i;
}

//释放DNS报文
void free_message(struct dns_message* msg)
{
	free(msg->header);
	dns_question* p = msg->question;
	while (p) {
		dns_question* tmp = p;
		p = p->next;
		free(tmp);
	}

	dns_resource_record* q = msg->answer;
	while (q) {
		dns_question* tmp = q;
		q = q->next;
		free(tmp);
	}

	free(msg);
}
