#pragma once

#include "header.h"
#include "default.h"
#include "system.h"
#include "dataStruct.h"

/*
	本头文件专门用于存放DNS报文结构体Message的定义，以及一切有关DNS报文的操作
	DNS 报文格式如下：
	+---------------------+
	|        Header       | 报文头，固定12字节，由结构体DNS_header存储
	+---------------------+
	|       Question      | 向域名服务器的查询请求，由结构体DNS_question存储
	+---------------------+
	|        Answer       | 对于查询问题的回复
	+---------------------+
	|      Authority      | 指向授权域名服务器
	+---------------------+
	|      Additional     | 附加信息
	+---------------------+
	后面三个部分由结构体DNS_resource_record存储
*/
typedef struct DNS_message {
	struct DNS_header* header;  // 报文头
	struct DNS_question* question;  // 查询问题
	struct DNS_resource_record* answer;  // 回答
	struct DNS_resource_record* authority;  // 授权
	struct DNS_resource_record* additional;  // 附加信息
}DNS_message;
/*
	报文头部结构体
	+-----------------------------------+
	|     事务id         |  标志位flags  |
	+-----------------------------------+
	|  问题数ques_num    | 回答数ans_num |
	+-----------------------------------+
	|  授权数auth_num    | 附加数add_num |
	+-----------------------------------+
	//DNS 报文的 ID 标识。对于请求报文和其对应的应答报文，该字段的值是相同的。通过它可以区分 DNS 应答报文是对哪个请求进行响应的。
	标志位结构体
	+---+---+---+---+---+---+---+---+----+
	|QR |OPCODE|AA |TC |RD |RA |Z  |RCODE|
	+---+---+---+---+---+---+---+---+----+
	QR：1位，查询/响应标志位。0表示查询报文，1表示响应报文。
	OPCODE：4位，操作码。通常取值为0，表示标准查询；取值为1，表示反向查询；取值为2，表示服务器状态查询。
	AA：1位，授权回答。该位在响应报文中有效，表示响应的服务器是一个权威服务器。
	TC：1位，截断。指示报文是否被截断。
	RD：1位，期望递归。表示发出查询的主机希望 DNS 服务器递归查询。
	RA：1位，可用递归。表示 DNS 服务器支持递归查询。
	Z：3位，保留字段。
	RCODE：4位，返回码。表示响应的返回状态。通常取值为0，表示没有错误；取值为3，表示名字错误。
 */
typedef struct DNS_header {
	uint16_t id;  // 事务id
	uint8_t qr : 1;  // 查询/响应标志位
	uint8_t opcode : 4;  // 操作码
	uint8_t aa : 1;  // 授权回答
	uint8_t tc : 1;  // 截断
	uint8_t rd : 1;  // 期望递归
	uint8_t ra : 1;  // 可用递归
	uint8_t z : 3;  // 保留
	uint8_t rcode : 4;  // 返回码

	uint16_t ques_num;  // 问题数
	uint16_t ans_num;  // 回答数(一般为3)
	uint16_t auth_num;  // 授权数
	uint16_t add_num;  // 附加数    

}DNS_header;

typedef struct DNS_question {
	char* q_name;  // 查询的域名
	uint16_t q_type;  // 查询类型
	uint16_t q_class;  // 查询类
	struct DNS_question* next;  // 下一个问题
}DNS_question;

// 资源数据的联合体
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |                             NAME                              |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |                             TYPE                              |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |                            CLASS                              |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |                             TTL                               |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |                         RDLENGTH                              |
//  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//  |                          RDATA                                |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
union ResourceData {
	/* IPv4 */
	struct {
		uint8_t IP_addr[4];
	} a_record;

	//-----------------------------------------
	/* IPv6 */
	struct {
		uint8_t IP_addr[16];
	} aaaa_record;
	//-----------------------------------------

	/* SOA：权威记录的起始 */
	struct {
		char* MName;        // 主服务器域名
		char* RName;        // 管理员邮箱
		uint32_t serial;    // 版本号
		uint32_t refresh;   // 刷新数据间隔
		uint32_t retry;     // 重试间隔
		uint32_t expire;    // 超时重传时间
		uint32_t minimum;   // 默认生存时间
	} soa_record;

	/* cname规范名称记录 */
	struct {
		char* name;
	} cname_record;
};

typedef struct DNS_resource_record {
	char* name;  // 域名
	uint16_t type;  // 类型
	uint16_t class;  // 类
	uint32_t ttl;  // 生存时间
	uint16_t data_len;  // 数据长度
	union ResourceData r_data;  // 资源内容
	struct DNS_resource_record* next;  // 下一个资源记录
}DNS_resource_record;

// 以下是DNS报文头部的各个字段的掩码
static const uint16_t QR_MASK = 0x8000;
static const uint16_t OPCODE_MASK = 0x7800;
static const uint16_t AA_MASK = 0x0400;
static const uint16_t TC_MASK = 0x0200;
static const uint16_t RD_MASK = 0x0100;
static const uint16_t RA_MASK = 0x0080;
static const uint16_t RCODE_MASK = 0x000F;

Address_Dns getDNSMessage(struct DNS_message* msg, Address_Dns buffer, Address_Dns start);
Address_Dns setDNSMessage(struct DNS_message* msg, Address_Dns buffer, struct Ips** head, int cnt);

Address_Dns get_header(struct NS_message* msg, Address_Dns buffer, Address_Dns start);
Address_Dns get_question(struct DNS_message* msg, Address_Dns buffer, Address_Dns start);
Address_Dns get_answer(struct DNS_message* msg, Address_Dns buffer, Address_Dns start);

Address_Dns get_domain_name(Address_Dns buffer, char* name, Address_Dns start);
Address_Dns set_domain_name(Address_Dns buffer, char* name);

Address_Dns set_header(struct DNS_message* msg, Address_Dns buffer, struct Ips** head, int cnt);
Address_Dns set_question(struct DNS_message* msg, Address_Dns buffer);
Address_Dns set_answer(struct DNS_message* msg, Address_Dns buffer, struct Ips** head, int cnt);

uint16_t add_list_id(uint16_t client_id, struct sockaddr_in client_addr, DNS_message* msg, int msg_size);

size_t get_bits(Address_Dns* buffer, int num);
void set_bits(Address_Dns* buffer, int len, int value);
void free_message(struct DNS_message* msg);
int isPtr(uint8_t* ptr);
