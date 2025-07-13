#include "../head/system.h"

//host-id表
char* host_path4 = "./host.txt";
char* LOG_PATH = "./log.txt";
char* host_path6 = "./host6.txt";

//模式
int debug_mode = 1;
int log_mode = 0;

//变量
int client_sock;
int server_sock;
int hostNum = 0;

//@brief 初始化系统
//@param argc 参数个数
//@param argv 参数列表
void init(int argc, char* argv[]) {
	remote_dns = "208.67.220.220";
	// 处理参数
	setConfigure(argc, argv);

	//打印帮助信息
	print_help_info();

	//初始化socket
	initSocket(53);

	//初始化Tire树
	initTire(node_list, NODE_NUM);

	//初始化节点列表
	initNodeList();

	//初始化ID映射表
	initIDMap();

	//初始化缓存
	initCache();

	//读入host文件
	readHost();

}

//@brief 初始化socket
//@param port 端口号
void initSocket(int port) {
	//初始化，否则无法运行socket
	WORD wVersion = MAKEWORD(2, 2);
	WSADATA wsadata;
	if (WSAStartup(wVersion, &wsadata) != 0) {
		return;
	}

	client_sock = socket(AF_INET, SOCK_DGRAM, 0);
	server_sock = socket(AF_INET, SOCK_DGRAM, 0);

	//初始化两个结构体以留空
	memset(&client_addr, 0, sizeof(client_addr));
	memset(&server_addr, 0, sizeof(server_addr));

	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = INADDR_ANY;     //INADDR_ANY表示本机的任意IP地址
	client_addr.sin_port = htons(port);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(remote_dns);      //远程主机地址
	server_addr.sin_port = htons(port);

	// 端口复用
	const int REUSE = 1;
	setsockopt(client_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&REUSE, sizeof(REUSE));

	if (bind(client_sock, (SOCKADDR*)&client_addr, addr_len) < 0) {
		printf("ERROR: Could not bind: %s\n", strerror(errno));
		exit(-1);
	}

	char* DNS_server = remote_dns;
	printf("\nDNS server: %s\n", DNS_server);
	printf("Listening on port %d\n\n", port);
}

void initCache() {
	/* 初始化LRU链表 */
	lru_head = malloc(sizeof(struct node));
	lru_head->next = NULL;
	lru_head->ips = NULL;
	lru_tail = lru_head;
	cache_size = 0;
}

void initNodeList() {
	for (int i = 0; i < NODE_NUM; i++) {
		node_list[i].isEnd = 0;
		for (int j = 0; j < 37; j++) {
			node_list[i].next[j] = 0;
		}
		node_list[i].pre = -1;
		node_list[i].ips = (struct Ips*)malloc(sizeof(struct Ips));
		initIps(node_list[i].ips);
	}
}

void setConfigure(int argc, char* argv[]) {
	argc--;
	for (int index = 1; index <= argc; index++) {
		// 调试模式
		if (strcmp(argv[index], "-d") == 0) {
			debug_mode = 1;
		}

		// 日志模式
		if (strcmp(argv[index], "-l") == 0) {
			log_mode = 1;
		}

		//输出基本信息 
		else if (strcmp(argv[index], "-i") == 0) {
			printf("far addr: %s (10.3.9.6, BUPT DNS) \n", remote_dns);
		}

		//设置远程DNS服务器
		else if (strcmp(argv[index], "-s") == 0) {
			char* addr = malloc(16);
			memset(addr, 0, 16);
			index++;
			memcpy(addr, argv[index], strlen(argv[index]) + 1);
			remote_dns = addr;
		}
	}
}

void print_help_info() {
	printf("-------------------------------------------------------------------------------\n");
	printf("|                          Welcome to use ourDNS!                             |\n");
	printf("| Please submit your query by terminal, and watch the answer in your terminal.|\n");
	printf("|                  Example: nslookup www.baidu.com 127.0.0.1                  |\n");
	printf("|     Arguments: -i:                  print basic information                 |\n");
	printf("|                -d:                  print debug information                 |\n");
	printf("|                -l:                  print log                               |\n");
	printf("|                -s [server_address]: set remote DNS server                   |\n");
	printf("-------------------------------------------------------------------------------\n");
}

void readHost() {
	FILE* host_ptr4 = fopen(host_path4, "r");
	FILE* host_ptr6 = fopen(host_path6, "r");
	if (!host_ptr4) {
		printf("can not open host-id list!\n");
		exit(1);
	}
	get_host_info_4(host_ptr4);
	get_host_info_6(host_ptr6);
}

void get_host_info_6(FILE* ptr) {
	int num = 0;

	while (!feof(ptr) && hostNum < MAX_HOST_SAVE) {
		if (num == 34) {
			int c = 0;
		}
		char* domain = malloc(256);     //域名
		char* ip = malloc(39);          //IP地址
		fscanf(ptr, "%s %s", ip, domain);   //读取文件
		if (strlen(domain) == 0) {
			break;
		}
		uint16_t ip_trans[8];
		//添加到前缀树，数组采用trie树
		transferIp6(ip, ip_trans);
		insert6(domain, ip_trans);
		num++;
		hostNum++;
	}

	if (debug_mode == 1) {
		printf("%d domain name address info has been loaded.\n\n", num);
	}
	fclose(ptr);
}

void get_host_info_4(FILE* ptr) {
	int num = 0;
	while (!feof(ptr) && hostNum < MAX_HOST_SAVE) {
		char* domain = malloc(256);     //域名
		char* ip = malloc(16);          //IP地址
		fscanf(ptr, "%s %s", ip, domain);   //读取文件
		if (strlen(domain) == 0) {
			break;
		}
		uint8_t ip_trans[4];
		transferIp(ip, ip_trans);
		//添加到前缀树，数组采用trie树
		insert4(domain, ip_trans);
		num++;
		hostNum++;
	}

	if (debug_mode == 1) {
		printf("%d domain name address info has been loaded.\n\n", num);
	}

	fclose(ptr);
}

void insert_host(struct DNS_message* msg) {
	struct  DNS_resource_record* temp = msg->answer;
	int len = msg->header->ans_num;
	FILE* host4 = fopen(host_path4, "a");
	FILE* host6 = fopen(host_path6, "a");

	if (host4 == NULL || host6 == NULL) {
		printf("can not open host-id list!\n");
		exit(1);
	}

	for (int i = 0; i < len; i++) {
		if (temp->type == RR_A) {
			host4 = fopen(host_path4, "a");
			fprintf(host4, "\n");
			fprintf(host4, "%d.%d.%d.%d %s", temp->r_data.a_record.IP_addr[0], temp->r_data.a_record.IP_addr[1], temp->r_data.a_record.IP_addr[2], temp->r_data.a_record.IP_addr[3], temp->name);
			uint8_t ip_trans[4];
			ip_trans[0] = temp->r_data.a_record.IP_addr[0];
			ip_trans[1] = temp->r_data.a_record.IP_addr[1];
			ip_trans[2] = temp->r_data.a_record.IP_addr[2];
			ip_trans[3] = temp->r_data.a_record.IP_addr[3];
			insert4(temp->name, ip_trans);
			fflush(host4);
			temp = temp->next;
		}
		else if (temp->type == RR_AAAA) {
			host6 = fopen(host_path6, "a");
			fprintf(host6, "\n");
			fprintf(host6, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x %s", temp->r_data.aaaa_record.IP_addr[0], temp->r_data.aaaa_record.IP_addr[1], temp->r_data.aaaa_record.IP_addr[2], temp->r_data.aaaa_record.IP_addr[3], temp->r_data.aaaa_record.IP_addr[4], temp->r_data.aaaa_record.IP_addr[5], temp->r_data.aaaa_record.IP_addr[6], temp->r_data.aaaa_record.IP_addr[7], temp->name);
			uint16_t ip_trans[8];
			ip_trans[0] = temp->r_data.a_record.IP_addr[0];
			ip_trans[1] = temp->r_data.a_record.IP_addr[1];
			ip_trans[2] = temp->r_data.a_record.IP_addr[2];
			ip_trans[3] = temp->r_data.a_record.IP_addr[3];
			ip_trans[4] = temp->r_data.a_record.IP_addr[4];
			ip_trans[5] = temp->r_data.a_record.IP_addr[5];
			ip_trans[6] = temp->r_data.a_record.IP_addr[6];
			ip_trans[7] = temp->r_data.a_record.IP_addr[7];
			insert6(temp->name, ip_trans);
			fflush(host6);
			temp = temp->next;
		}
	}
}

void writeLog(char* IP, char* domain) {
	FILE* log_ptr = fopen(LOG_PATH, "a");
	if (!log_ptr) {
		printf("can not open host!\n");
		exit(1);
	}
	fprintf(log_ptr, "%s %s\n", IP, domain);
	fclose(log_ptr);
}
