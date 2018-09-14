#include "udp_client.h"

/* 唤醒数据接收线程互斥量 */
pthread_mutex_t recv_thread_mutex = PTHREAD_MUTEX_INITIALIZER;
/* 唤醒数据接收线程条件变量 */
pthread_cond_t recv_thread_cond = PTHREAD_COND_INITIALIZER;

/* 打印在线用户 */
static void print_onlines(int ip, int port);
 
/* 打印收到的消息 */
static void printf_msg(int ip, int port, const char *msg);

/* 数据接收线程 */
void* udp_recv_thread(void *p_udp_fd)
{	
	/* 等待被唤醒 */
	pthread_mutex_lock(&recv_thread_mutex);
	pthread_cond_wait(&recv_thread_cond, &recv_thread_mutex);
	pthread_mutex_unlock(&recv_thread_mutex);
	
	int udp_fd = *(int*)p_udp_fd;
	
	Net_packet packet;
	
	/* 设置服务器地址 */
	packet.dst_port = htons(UDP_PORT);
	inet_pton(AF_INET, SERVER_IP, &packet.dst_ip);

	while(1)
	{
		/* 设置服务器地址 */
		recv_data(udp_fd, &packet); // 接收数据包
		
		/* 解析指令 */
		switch(packet.data_type)
		{
		case DATA_CHAT:  // 打印收到的消息
			printf_msg(packet.src_ip, packet.src_port, packet.data);
			break;
		case DATA_ONLINE: // 打印在线用户
			print_onlines(packet.src_ip, packet.src_port);
			break;
		case DATA_LOGOUT: // 退出登录
			return NULL;
		default:
			break;
		}
	}

	return NULL;
}

/* 打印在线用户 */
void print_onlines(int ip, int port)
{
	if (ip < 0 || port < 0)
	{
		printf("[没有其他在线用户]\n");
		return;
	}
	
	char ip_str[20];
	
	printf("[在线用户: IP:%s 端口:%d]\n",
		   inet_ntop(AF_INET, &ip, ip_str, sizeof(ip_str)),
		   ntohs(port));
}

/* 打印收到的消息 */
void printf_msg(int ip, int port, const char *msg)
{
	if (ip < 0 || port < 0)
	{
		return;
	}

	/* 获取目前时间 */
    time_t t;
    struct tm *pt;
    time(&t);
    pt = gmtime(&t);
	
	char ip_str[20];
	
	printf("[收到消息! %d:%d:%d]\n"
		   "[%s]\n"
		   "[来自用户: IP:%s 端口:%d]\n",
		   8 + pt->tm_hour,pt->tm_min, pt->tm_sec, 
		   msg,
		   inet_ntop(AF_INET, &ip, ip_str, sizeof(ip_str)),
		   ntohs(port));
}