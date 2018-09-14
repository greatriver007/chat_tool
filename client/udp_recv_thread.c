#include "udp_client.h"

/* 打印在线用户 */
static void print_onlines(int ip, int port);
 
/* 打印收到的消息 */
static void printf_msg(int ip, int port, const char *msg);


/* 数据接收线程 */
void* udp_recv_thread(void *p_udp_fd)
{	
	pthread_detach(pthread_self());	//设置分离属性

	int udp_fd = *(int*)p_udp_fd;
	
	Net_packet packet;

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
		   8 + pt->tm_hour,pt->tm_min, pt->tm_sec, msg,
		   inet_ntop(AF_INET, &ip, ip_str, sizeof(ip_str)),
		   ntohs(port));
}