#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include "header.h"
#include "protocol.h"

/* 初始化TCP客户端 */
extern int tcp_client_init();

/* 文件发送线程 */
extern void* tcp_send_thread(void *tcp_fd);

/* 文件接收线程 */
extern void* tcp_recv_thread(void *tcp_fd); 

#endif

