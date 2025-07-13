#pragma once

#include "header.h"
#include "default.h"
#include "dataStruct.h"
#include "dnsStruct.h"
#include "system.h"

SOCKET client_sock;			// 客户端socket
SOCKET server_sock;			// 服务端socket

struct sockaddr_in client_addr; // 客户端地址
struct sockaddr_in server_addr; // 服务端地址

int addr_len;               // 地址长度
char* remote_dns;			// 远程主机ip地址

void poll();               // 非阻塞模式&轮询机制
void timeout_handle();     // 超时处理
void receiveClient();      // 接收客户端数据
void receiveServer();      // 接收服务器数据
void closeServer();        // 关闭连接
