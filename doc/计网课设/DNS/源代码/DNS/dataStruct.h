#pragma once

#include "header.h"
#include "default.h"
#include "system.h"
#include "server.h"

/* IP地址结构体 */
typedef struct ip_list_node {
	union {
		uint8_t ipv4[4];		// IPv4地址
		uint16_t ipv6[8];		// IPv6地址
	} addr;
	int version;				// 4 or 6
	struct ip_list_node* next;	// 下一条IP地址记录
} ip_list_node, * ip_list;

/* ID转换结构体 */
typedef struct {
	uint16_t client_ID;				// 客户端ID
	int expire_time;				// 过期时间
	struct dns_message* msg;		// DNS报文
	int msg_size;					// 报文大小
	struct sockaddr_in client_addr;	// 客户端地址
} ID_conversion;

/* Trie树结构体 */
typedef struct trie_node {
	int is_end;			// 是否是单词的结尾
	uint16_t next[38];	// 指向下一个节点的指针数组
	ip_list ip_list;	// IP地址链表
} trie_node, * trie;

/* LRU链表结构体 */
typedef struct lru_node {
	uint16_t version;			// 4 or 6
	char domain[MAX_SIZE];		// 域名
	ip_list ip_list;			// 域名对应的IP地址链表
	struct lru_node* prev;		// 前驱节点
	struct lru_node* next;		// 后继节点
} lru_node;

trie_node node_list[NODE_NUM];			// 节点列表
ID_conversion ID_list[ID_LIST_SIZE];	// ID转换列表
size_t list_size;						// 节点列表大小

lru_node* lru_head;	// LRU链表头
lru_node* lru_tail;	// LRU链表尾
size_t cache_size;	// 缓存大小

//IP链表相关
void init_ip_list(ip_list head);

//IP地址转换
void ipv4_to_bytes(const char* ipv4_str, uint8_t* ipv4_bytes);
void ipv6_to_bytes(const char* ipv6_str, uint16_t* ipv6_bytes);
int hex_to_int(char c);

//Trie树的相关操作
void init_trie(trie_node* root, int len);
int get_index(char c);
void insert4(char* domain, uint8_t* ip);
void insert6(char* domain, uint16_t* ip);
int search(char* domain, ip_list head);

//缓存相关操作
void init_cache();
void update_cache(char* domain, ip_list head);
int search_cache(char* domain, ip_list head);
void delete_cache();
