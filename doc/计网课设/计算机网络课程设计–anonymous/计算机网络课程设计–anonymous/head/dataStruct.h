#pragma once

#include "header.h"
#include "default.h"
#include "system.h"
#include "server.h"

/* IP地址结构体 */
typedef struct Ips {
	int type;			// IPv4 or IPv6
	uint8_t ip[4];		// IPv4地址
	uint16_t ip6[8];	// IPv6地址
	int isFilledIp4;	// 是否为IPv4地址
	int isFilledIp6;	// 是否为IPv6地址
	struct Ips* next;	// 下一条IP地址记录
} Ips;

/* ID转换结构体 */
typedef struct {
	uint16_t client_ID;				// 客户端ID
	int expire_time;				// 过期时间
	struct DNS_message* msg;		// DNS报文
	int msg_size;					// 报文大小
	struct sockaddr_in client_addr;	// 客户端地址
} ID_conversion;

/* Tire树结构体 */
typedef struct TireNode {
	int isEnd;		// 是否是单词的结尾
	int next[38];	// 指向下一个节点的指针数组
	Ips* ips;		// IP地址链表
	int pre;		// 前一个节点的下标
} TireNode, Tire;

/* LRU链表结构体 */
typedef struct node {
	uint16_t type;			// IPv4 or IPv6
	char domain[MAX_SIZE];	// 域名
	Ips* ips;				// 域名对应的IP地址链表
	struct node* next;		// 下一个节点
} lru_node;

Tire node_list[NODE_NUM];	// 节点列表
ID_conversion ID_list[ID_LIST_SIZE]; // ID转换列表
int list_size;		// 节点列表大小

lru_node* lru_head;	// LRU链表头
lru_node* lru_tail;	// LRU链表尾
int cache_size;		// 缓存大小

//IP地址转换
void transferIp(char* originIP, uint8_t* transIP);
void transferIp6(char* originIP, uint16_t* transIP);
int hex_to_int(char c);

//tire树的相关操作
void initTire(Tire* root, int len);
int get_index(char c);
void insert4(char* domain, uint8_t* ip);
void insert6(char* domain, uint16_t* ip);
int search(char* domain, Ips** head);

//缓存相关操作
void init_cache();
void update_cache(char* domain, Ips* head);
int search_cache(char* domain, Ips** head);
void delete_cache();
