#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "header.h"

extern int tcp_server_init(package_t *package);			// 初始化TCP客户端
extern void *tcp_server_file_send_thread(void *arg); 	// 文件发送线程函数
extern void *tcp_server_file_recv_thread(void *arg); 	// 文件接收线程函数

#endif

