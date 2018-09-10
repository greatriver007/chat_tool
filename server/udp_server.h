#ifndef __UDP_SERVER_H__
#define __UDP_SERVER_H__

#include "header.h"

extern int udp_server_init(package_t *package); // 初始化UDP客户端
extern void *udp_server_msg_repost_thread(void * arg); // 消息转发线程函数

#endif

