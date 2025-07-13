#pragma once

#define NODE_NUM 65535				// 字典树的最大结点数
#define MAX_SIZE 300				// 域名最大长度
#define MAX_CACHE 100				// 缓存最大容量
#define DEFAULT_PORT 53				// 监听端口号
#define BUFFER_SIZE 1500			// DNS报文的最大尺寸
#define ID_LIST_SIZE 128			// ID映射表大小
#define ID_EXPIRE_TIME 1			// ID过期时间
#define MAX_HOST_SAVE 15000			// 最大保存的host-ip对数

typedef uint8_t* dns_addr;

/* Resource Record类型 */
#define RR_A 1
#define RR_CNAME 5
#define RR_SOA 6
#define RR_PTR 12
#define RR_MX 15
#define RR_TXT 16
#define RR_AAAA 28
