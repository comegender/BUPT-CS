#pragma once

#include "header.h"
#include "server.h"
#include "dataStruct.h"
#include "dnsStruct.h"

char* LOG_PATH;						// 日志文件目录

//全局变量
int debug_mode;
int log_mode;
int selectPort;						// 选择端口 
int host_num;						// host数量	

//函数声明
void init(int argc, char* argv[]);				// 初始化系统
void config(int argc, char* argv[]);		// 设置配置
void init_node_list();							// 初始化节点列表
void print_help_info();							// 打印帮助信息
void init_socket(int port);						// 初始化socket
void read_host();								// 读入host文件
void init_id_map();								// 初始化ID映射表
void get_host_info_4();							// 获取host信息	
void get_host_info_6();							// 获取host信息
void write_log(char* ip, char* domain);			// 写入日志
void insert_host(struct dns_message* msg);		// 插入host
