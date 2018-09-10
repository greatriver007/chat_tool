#ifndef __UDP_CLIENT_H__
#define __UDP_CLIENT_H__

#include "header.h"

extern int udp_client_init(package_t *package); 		// 初始化UDP客户端
extern void *udp_client_msg_send_thread(void * arg); 	// 消息发送线程函数
extern void *udp_client_msg_recv_thread(void * arg); 	// 消息接收线程函数

#endif

