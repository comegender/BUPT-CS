#include "server.h"

int addr_len = sizeof(struct sockaddr_in);

void poll()
{
	u_long block_mode = 1;
	int server_result = ioctlsocket(server_sock, FIONBIO, &block_mode);
	int client_result = ioctlsocket(client_sock, FIONBIO, &block_mode);

	if (server_result == SOCKET_ERROR || client_result == SOCKET_ERROR) {
		// 设置失败
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(server_sock);
		closesocket(client_sock);
		WSACleanup();
		return;
	}

	struct pollfd fds[2];

	for (;;) {
		timeout_handle();
		fds[0].fd = client_sock;
		fds[0].events = POLLIN;
		fds[1].fd = server_sock;
		fds[1].events = POLLIN;

		int result = WSAPoll(fds, 2, 5);
		if (result == SOCKET_ERROR)
			printf("ERROR WSAPoll: %d.\n", WSAGetLastError());
		else if (result > 0) {
			if (fds[0].revents & POLLIN)
				receiveClient();
			if (fds[1].revents & POLLIN)
				receiveServer();
		}
	}
}

void timeout_handle()
{
	for (int i = 0; i < ID_LIST_SIZE; i++) {
		if (ID_list[i].expire_time < time(NULL) && ID_list[i].expire_time != 0) {       //expire_time为0时默认为此项还未被使用
			uint8_t buffer[BUFFER_SIZE];
			set_dns_message(ID_list[i].msg, buffer, NULL, 0);
			uint16_t old_ID = htons(ID_list[i].client_ID);
			memcpy(buffer, &old_ID, sizeof(uint16_t));
			sendto(client_sock, buffer, ID_list[i].msg_size, 0, (struct sockaddr*)&ID_list[i].client_addr, addr_len);
			if (debug_mode == 1) {
				time_t timep;
				time(&timep);
				printf("%s", ctime(&timep));
				printf("ID %d from %s:%d timeout.\n", ID_list[i].client_ID, inet_ntoa(ID_list[i].client_addr.sin_addr), ntohs(ID_list[i].client_addr.sin_port));
			}
			//清空该ID的信息-----------------------------------------
			ID_list[i].client_ID = 0;
			ID_list[i].expire_time = 0;
			ID_list[i].msg = NULL;
			ID_list[i].msg_size = 0;
			memset(&(ID_list[i].client_addr), 0, sizeof(struct sockaddr_in));
			ID_list[i].expire_time = 0;
		}
	}
}

void receiveClient()
{
	uint8_t buffer[BUFFER_SIZE];        // 接收的报文
	uint8_t buffer_to_client[BUFFER_SIZE];    // 回复给客户端的报文
	dns_message msg;  
	msg.additional = NULL;
	msg.question = NULL;
	msg.authority = NULL;
	msg.header = NULL;
	msg.answer = NULL;

	// 报文结构体
	ip_list_node* head = (ip_list_node*)malloc(sizeof(ip_list_node));    // 头指针
	head->next = NULL;                // 初始化
	head->version = 0;                   // 初始化

	int msg_size = -1;                  // 报文大小
	int found_cnt = 0;                   // 是否查到

	//ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,struct sockaddr *src_addr, socklen_t *addrlen);
	//sockfd：指定接收数据的套接字描述符。
	//buf：指向缓冲区的指针，用于存储接收到的数据。
	//len：缓冲区的大小，即最多能接收的数据量。
	//flags：提供额外的控制方式。常用的标志包括MSG_PEEK（查看数据但不从系统缓冲区中移除）、MSG_WAITALL（等待所有请求的数据）等。
	//src_addr：（可选）指向struct sockaddr的指针，用于存储发送方地址信息。
	//addrlen：指向存储地址长度的变量的指针。
	msg_size = recvfrom(client_sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);

	if (msg_size < 0) {
		printf("ERROR: Could not receive client: %s\n", strerror(errno));
		return;
	}
	else {
		uint8_t* start = buffer;
		if (debug_mode == 1)
			printf("\n------------------DNS data------------------\n");

		//解析报文
		get_dns_message(&msg, buffer, start);

		if(debug_mode == 1){
			time_t timep;
			time(&timep);
			printf("%s", ctime(&timep));
			printf("ID %d from Client %s:%d\n", msg.header->id, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			printf("Target Domain: %s\n\n", msg.question->q_name);
		}

		if (msg.question->q_type == RR_A)
			head->version = 4;

		if (msg.question->q_type == RR_AAAA)
			head->version = 6;

		//判断
		if (msg.question->q_type == RR_A || msg.question->q_type == RR_AAAA) {
			//从缓存查找
			found_cnt = search_cache(msg.question->q_name, head);

			//没查到
			if (found_cnt == 0) {
				if (debug_mode == 1)
					printf("Address not found in cache.\n");
				found_cnt = search(msg.question->q_name, head);
				if (found_cnt == 0 && debug_mode == 1)
					printf("Address not found in host file.\n");
			}
		}

		if (found_cnt == 0) {
			/* 发送给外部服务器 */
			//放入list-id表中,获取新的id
			uint16_t new_id = add_list_id(msg.header->id, client_addr, &msg, msg_size);
			uint16_t new_id_temp = new_id;
			if (new_id != ID_LIST_SIZE) {
				new_id = htonl(new_id);
				memcpy(buffer, &new_id, 2);
				//发送数据
				sendto(server_sock, buffer, msg_size, 0, (struct sockaddr*)&server_addr, addr_len);
				if(debug_mode == 1){
					time_t timep;
					time(&timep);
					printf("%s", ctime(&timep));
					printf("relay to far DNS server.\n");
					printf("NewID: %d, OldID: %d\n\n", new_id_temp, msg.header->id);
				}
			}
			return;
		}

		dns_addr end;
		end = set_dns_message(&msg, buffer_to_client, &head, found_cnt);
		//发送数据
		int len = end - buffer_to_client; //结尾减去开头
		sendto(client_sock, buffer_to_client, len, 0, (struct sockaddr*)&client_addr, addr_len);
		printf("Domain: %s finished.\n\n", msg.question->q_name);
	}
}

void receiveServer()
{
	uint8_t buffer[BUFFER_SIZE];        // 接收的报文
	dns_message msg;
	msg.additional = NULL;
	msg.question = NULL;
	msg.authority = NULL;
	msg.header = NULL;
	msg.answer = NULL;
	int msg_size = -1;                  // 报文大小

	msg_size = recvfrom(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &addr_len);
	/* 将DNS应答报文转发回客户端 */
	if (msg_size < 0) {
		printf("ERROR: Could not receive server: %s\n", msg_size);
		return;
	}
	else {
		get_dns_message(&msg, buffer, buffer);

		if(debug_mode == 1){
			time_t timep;
			time(&timep);
			printf("%s", ctime(&timep));
			printf("ID %d from Server %s:%d\n", msg.header->id, inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
			printf("Target Domain: %s\n\n", msg.question->q_name);
		}

		/* ID转换 */
		uint16_t ID = msg.header->id;
		uint16_t old_ID = htons(ID_list[ID].client_ID);
		memcpy(buffer, &old_ID, sizeof(uint16_t));        //把待发回客户端的包ID改回原ID

		struct sockaddr_in ca = ID_list[ID].client_addr;

		//清空该ID的信息-----------------------------------------
		ID_list[ID].expire_time = 0;
		ID_list[ID].client_ID = 0;
		ID_list[ID].msg = NULL;
		ID_list[ID].msg_size = 0;
		memset(&(ID_list[ID].client_addr), 0, sizeof(struct sockaddr_in));

		//-----------------------------------------------------
		/*将外部DNS服务器查到的结果写入缓存*/
		if (msg.header->ans_num > 0 && (msg.answer->type == RR_A || msg.answer->type == RR_AAAA)) {
			ip_list_node* update = (ip_list_node*)malloc(sizeof(ip_list_node));
			update->next = NULL;
			ip_list_node* p = update;
			dns_resource_record* q = msg.answer;
			while (q) {
				p->next = (ip_list_node*)malloc(sizeof(ip_list_node));
				p->next->next = NULL;
				if (q->type == RR_A) {
					p->next->version = 4;
					for (int i = 0; i < 4; i++) {
						p->next->addr.ipv4[i] = q->r_data.a_record.IP_addr[i];
					}
				}
				else if (q->type == RR_AAAA) {
					p->next->version = 6;
					memcpy(p->next->addr.ipv6, q->r_data.aaaa_record.IP_addr, sizeof(uint8_t) * 16);
					for (int i = 0; i < 8; i++) {
						p->next->addr.ipv6[i] = ntohs(p->next->addr.ipv6[i]);
					}
				}
				else {
					p->next = NULL;
					break;
				}
				p = p->next;
				q = q->next;
			}
			update_cache(msg.question->q_name, update);
		}

		//判断是否存在在树里
		int isFound = 0;
		ip_list_node* temp = (ip_list_node*)malloc(sizeof(ip_list_node));
		if (msg.question->q_type == RR_A)
			temp->version = 4;
		if (msg.question->q_type == RR_AAAA)
			temp->version = 6;

		//代表第一次查找
		if (msg.answer != NULL)
		{
			isFound = search(msg.answer->name, temp);
			if (isFound == 0) {
				insert_host(&msg);
			}
		}
		sendto(client_sock, buffer, msg_size, 0, (struct sockaddr*)&ca, addr_len);
		printf("Domain: %s finished.\n\n", msg.question->q_name);
	}
}

void closeServer()
{
	closesocket(client_sock);
	closesocket(server_sock);
	WSACleanup();//释放资源
}
