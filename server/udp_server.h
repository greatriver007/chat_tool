#ifndef UDP_SERVER_H
#define UDP_SERVER_H

extern "C"
{
#include <arpa/inet.h>
}

int udp_server_init();			//初始化udp服务器
void *udp_routine(void *arg);	//消息发送线程函数

#endif
