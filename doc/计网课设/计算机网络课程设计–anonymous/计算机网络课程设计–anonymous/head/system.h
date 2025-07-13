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
int hostNum;						// host数量	

//函数声明
void init(int argc, char* argv[]);				// 初始化系统
void setConfigure(int argc, char* argv[]);		// 设置配置
void initNodeList();							// 初始化节点列表
void print_help_info();							// 打印帮助信息
void initSocket();								// 初始化socket
void readHost();								// 读入host文件
void initCache();								// 初始化缓存
void initIDMap();								// 初始化ID映射表
void get_host_info_4(FILE* ptr);				// 获取host信息	
void get_host_info_6(FILE* ptr);				// 获取host信息
void write_log(char* log);						// 写入日志
void insert_host(struct DNS_message* msg);		// 插入host
