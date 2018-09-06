#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#ifdef _cplusplus
extern "C"
{
#endif

int udp_init(int port);				//初始化udp网络

void *udp_send_routine(void *arg);	//消息发送线程函数
void *udp_recv_routine(void *arg);	//消息接受线程函数

void login(int udp_fd);
void logout(int udp_fd);

#ifdef _cplusplus
}
#endif

#endif
