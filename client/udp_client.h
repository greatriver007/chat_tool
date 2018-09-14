#ifndef __UDP_CLIENT_H__
#define __UDP_CLIENT_H__

#include "header.h"
#include "protocol.h"

/* 初始化UDP客户端 */
extern int udp_client_init();

/* 数据发送线程 */
extern void* udp_send_thread(void *udp_fd);

/* 数据接收线程 */
extern void* udp_recv_thread(void *udp_fd); 

/* 数据接收函数 */
extern void recv_data(int fd, Net_packet* packet);

/* 数据发送函数 */
extern void send_data(int fd, const Net_packet* packet);

#endif

