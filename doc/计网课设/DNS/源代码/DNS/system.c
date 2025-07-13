#include "system.h"

//host-id表
char* host_path4 = "host_ipv4.txt";
char* LOG_PATH = "log.txt";
char* host_path6 = "host_ipv6.txt";

//模式
int debug_mode = 1;
int log_mode = 0;

//变量
SOCKET client_sock;
SOCKET server_sock;
int host_num = 0;

//@brief 初始化系统
//@param argc 参数个数
//@param argv 参数列表
void init(int argc, char* argv[])
{
	remote_dns = "208.67.220.220";

	// 处理参数
	config(argc, argv);

	//打印帮助信息
	print_help_info();

	//初始化socket
	init_socket(53);

	//初始化trie_node树
	init_trie(node_list, NODE_NUM);

	//初始化节点列表
	init_node_list();

	//初始化ID映射表
	init_id_map();

	//初始化缓存
	init_cache();

	//读入host文件
	read_host();
}

//@brief 初始化socket
//@param port 端口号
void init_socket(int port)
{
	//初始化，否则无法运行socket
	WORD wVersion = MAKEWORD(2, 2);
	WSADATA wsadata;
	if (WSAStartup(wVersion, &wsadata) != 0)
		return;

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

	char* dns_server = remote_dns;
	printf("\ndns server: %s\n", dns_server);
	printf("Listening on port %d\n\n", port);
}

void init_node_list()
{
	for (int i = 0; i < NODE_NUM; i++) {
		node_list[i].is_end = 0;
		for (int j = 0; j < 37; j++)
			node_list[i].next[j] = 0;
		node_list[i].ip_list = malloc(sizeof(ip_list_node));
		init_ip_list(node_list[i].ip_list);
	}
}

void config(int argc, char* argv[])
{
	argc--;
	for (int index = 1; index <= argc; index++) {
		// 调试模式
		if (strcmp(argv[index], "-d") == 0)
			debug_mode = 1;

		// 日志模式
		else if (strcmp(argv[index], "-l") == 0)
			log_mode = 1;

		//输出基本信息 
		else if (strcmp(argv[index], "-i") == 0)
			printf("far addr: %s (10.3.9.45, BUPT DNS) \n", remote_dns);

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

void print_help_info()
{
	printf("-------------------------------------------------------------------------------\n");
	printf("|                          Welcome to use our DNS!                            |\n");
	printf("| Please submit your query by terminal, and watch the answer in your terminal.|\n");
	printf("|                  Example: nslookup www.baidu.com 127.0.0.1                  |\n");
	printf("|     Arguments: -i:                  print basic information                 |\n");
	printf("|                -d:                  print debug information                 |\n");
	printf("|                -l:                  print log                               |\n");
	printf("|                -s [server_address]: set remote DNS server                   |\n");
	printf("-------------------------------------------------------------------------------\n");
}

void read_host()
{
	get_host_info_4();
	get_host_info_6();
}

void get_host_info_4()
{
	int num = 0;
	FILE* host4 = fopen(host_path4, "r");
	if (host4 == NULL) {
		printf("Failed to open file host_ipv4!\n");
		exit(1);
	}
	while (!feof(host4) && host_num < MAX_HOST_SAVE) {
		char* domain = malloc(256);     //域名
		char* ip = malloc(16);          //IP地址
		fscanf(host4, "%s %s", ip, domain);   //读取文件
		if (strlen(domain) == 0)
			break;
		uint8_t ip_trans[4];
		ipv4_to_bytes(ip, ip_trans);

		//添加到前缀树，数组采用Trie树
		insert4(domain, ip_trans);
		num++;
		host_num++;
	}

	if (debug_mode == 1)
		printf("%d domain name address info has been loaded.\n\n", num);

	fclose(host4);
}

void get_host_info_6()
{
	int num = 0;
	FILE* host6 = fopen(host_path6, "r");
	if (host6 == NULL) {
		printf("Failed to open file host_ipv6!\n");
		exit(1);
	}
	while (!feof(host6) && host_num < MAX_HOST_SAVE) {
		char* domain = malloc(256);     //域名
		char* ip = malloc(39);          //IP地址
		fscanf(host6, "%s %s", ip, domain);   //读取文件
		if (strlen(domain) == 0)
			break;
		uint16_t ip_trans[8];

		//添加到前缀树，数组采用Trie树
		ipv6_to_bytes(ip, ip_trans);
		insert6(domain, ip_trans);
		num++;
		host_num++;
	}

	if (debug_mode == 1)
		printf("%d domain name address info has been loaded.\n\n", num);
	fclose(host6);
}

void insert_host(struct dns_message* msg)
{
	struct dns_resource_record* temp = msg->answer;
	int len = msg->header->ans_num;
	FILE* host4 = fopen(host_path4, "a");
	FILE* host6 = fopen(host_path6, "a");

	if (host4 == NULL) {
		printf("Failed to open file host_ipv4!\n");
		exit(1);
	}

	if (host6 == NULL) {
		printf("Failed to open file host_ipv6!\n");
		exit(1);
	}

	for (int i = 0; i < len; i++) {
		if (temp->type == RR_A) {
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
			fprintf(host6, "\n");
			fprintf(host6, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x %s", temp->r_data.aaaa_record.IP_addr[0], temp->r_data.aaaa_record.IP_addr[1], temp->r_data.aaaa_record.IP_addr[2], temp->r_data.aaaa_record.IP_addr[3], temp->r_data.aaaa_record.IP_addr[4], temp->r_data.aaaa_record.IP_addr[5], temp->r_data.aaaa_record.IP_addr[6], temp->r_data.aaaa_record.IP_addr[7], temp->name);
			uint16_t ip_trans[8];
			ip_trans[0] = temp->r_data.aaaa_record.IP_addr[0];
			ip_trans[1] = temp->r_data.aaaa_record.IP_addr[1];
			ip_trans[2] = temp->r_data.aaaa_record.IP_addr[2];
			ip_trans[3] = temp->r_data.aaaa_record.IP_addr[3];
			ip_trans[4] = temp->r_data.aaaa_record.IP_addr[4];
			ip_trans[5] = temp->r_data.aaaa_record.IP_addr[5];
			ip_trans[6] = temp->r_data.aaaa_record.IP_addr[6];
			ip_trans[7] = temp->r_data.aaaa_record.IP_addr[7];
			insert6(temp->name, ip_trans);
			fflush(host6);
			temp = temp->next;
		}
	}
	fclose(host4);
	fclose(host6);
}

void write_log(char* ip, char* domain)
{
	FILE* log_ptr = fopen(LOG_PATH, "a");
	if (!log_ptr) {
		printf("can not open host!\n");
		exit(1);
	}
	fprintf(log_ptr, "%s %s\n", ip, domain);
	fclose(log_ptr);
}
