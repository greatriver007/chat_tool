#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "protocol.h"

/* 数据接收函数 */
void recv_data(int fd, Net_packet* packet)
{
	struct sockaddr_in addr;
	socklen_t addr_size = sizeof(addr);
	
	/* 接收数据 */
	recvfrom(fd, packet, sizeof(Net_packet), 0, 
			 (struct sockaddr*)&addr, &addr_size);
	
	/* 设置发送者地址 */
	packet->src_ip = addr.sin_addr.s_addr;
	packet->src_port = addr.sin_port;
}


/* 数据发送函数 */
void send_data(int fd, const Net_packet* packet)
{	
	/* 设置目标地址 */
	struct sockaddr_in addr;	
	addr.sin_family = AF_INET;
	addr.sin_port = packet->dst_port;		
	addr.sin_addr.s_addr = packet->dst_ip;
		
	/* 发送消息 */
	sendto(fd, packet, sizeof(Net_packet), 0, 
		   (struct sockaddr*)(&addr), sizeof(addr));
}
