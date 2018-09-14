#include "udp_client.h"

/* 初始化UDP客户端 */
int udp_client_init()
{	
	struct sockaddr_in udp_addr;

	/* 创建UDP套接字 */
	int udp_fd = socket(AF_INET, SOCK_DGRAM, 0);	
	if (udp_fd < 0)
	{
#ifdef DEBUG
		perror("udp_client_init: 创建UDP套接字失败");
#endif
		exit(1);
	}
	
	/* 设置UDP套接字地址 */
	udp_addr.sin_family = AF_INET;			
	udp_addr.sin_port = htons(SERVER_PORT);		
	inet_pton(AF_INET, SERVER_IP, &udp_addr.sin_addr);

	/* 允许重用地址 */ 
	int opt = 1;
	if (setsockopt(udp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0)
	{
#ifdef DEBUG
		perror("udp_client_init: 允许重用地址失败");
#endif
		close(udp_fd);
		exit(0);
	}

	/* 允许发送广播数据  */
	if (setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) != 0)
	{
#ifdef DEBUG
		perror("udp_client_init: 允许发送广播数据失败");
#endif
		close(udp_fd);
		exit(0);
	}
	
	return udp_fd;
}


/* 数据接收函数 */
void recv_data(int fd, Net_packet* packet)
{
	struct sockaddr_in addr;
	socklen_t addr_size = sizeof(addr);
	
	/* 接收数据 */
	recvfrom(fd, packet, sizeof(Net_packet), 0, 
			 (struct sockaddr*)&addr, &addr_size);
}


/* 数据发送函数 */
void send_data(int fd, const Net_packet* packet)
{	
	/* 设置服务器地址 */
	struct sockaddr_in addr;	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);		
	inet_pton(AF_INET, SERVER_IP, &addr.sin_addr);	
		
	/* 发送消息 */
	sendto(fd, packet, sizeof(Net_packet), 0, 
		   (struct sockaddr*)(&addr), sizeof(addr));
}
