#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "server.h"
#include "protocol.h"
#include "addr_list.h"

/* 初始化服务器 */
static int server_init(struct sockaddr_in* server_addr);
				
/* 数据接收函数 */
static void recv_data(int fd, Net_packet* packet);

/* 数据发送函数 */
static void send_data(int fd, const Net_packet* packet);
	
/* 发送在线用户函数 */
static void send_onlines(int server_fd, const addr_t* addr_list,
						 Net_packet* packet);
/* 增加用户 */
static void add_user(int server_fd, addr_t* addr_list,
					 Net_packet* packet);
/* 删除用户 */
static void remove_user(int server_fd, addr_t* addr_list,
						Net_packet* packet);
			
/* 服务器例程 */
void server_routine()
{
	Net_packet packet;	
	struct sockaddr_in server_addr;
	
	int server_fd = server_init(&server_addr); // 初始化服务器
	addr_t* addr_list = create_list(); // 创建在线用户列表
	
	while (1)
	{
		recv_data(server_fd, &packet); // 接收数据包
		
		/* 解析数据包 */
		switch(packet.data_type)
		{
		case DATA_CHAT:  // 转发消息
			send_data(server_fd, &packet); 
			break;
		case DATA_ONLINE: // 发送在线用户
			send_onlines(server_fd, addr_list, &packet);
			break;
		case DATA_LOGIN:  // 增加用户
			add_user(server_fd, addr_list, &packet);
			break;
		case DATA_LOGOUT: // 删除用户
			remove_user(server_fd, addr_list, &packet);
			break;
		default: 
			break;
		}
	}

	destroy_list(&addr_list); // 销毁在线用户列表	
	close(server_fd); // 关闭服务器
}


/* 初始化服务器 */
int server_init(struct sockaddr_in* server_addr)
{
	/* 创建UDP套接字 */
	int server_fd = socket(AF_INET, SOCK_DGRAM, 0);	
	if (server_fd < 0)
	{
		perror("server_init: 创建UDP套接字失败");
		exit(1);
	}

	/* 设置UDP套接字地址 */
	server_addr->sin_family = AF_INET;			
	server_addr->sin_port = htons(SERVER_PORT);		
	(server_addr->sin_addr).s_addr = htonl(INADDR_ANY);
	
	/* 允许重用地址 */ 
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, 
			&opt, sizeof(opt)) != 0)
	{
		perror("server_init: 允许重用地址失败");
		close(server_fd);
		exit(1);
	}

	/* 允许发送广播数据  */
	if (setsockopt(server_fd, SOL_SOCKET, SO_BROADCAST,
			&opt, sizeof(opt)) != 0)
	{
		perror("server_init: 允许发送广播数据失败");
		close(server_fd);
		exit(1);
	}
	
	/* 绑定套接字地址 */
	if (bind(server_fd, (struct sockaddr*)server_addr, 
		sizeof(*server_addr)) != 0)
	{
		perror("server_init: 绑定套接字地址失败");
		close(server_fd);
		exit(1);
	}
	
	return server_fd;
}


/* 发送在线用户函数 */
void send_onlines(int server_fd, const addr_t* addr_list,
				  Net_packet* packet)
{	
	const addr_t *p_addr = addr_list->next;
	
	/* 设置目标地址 */
	packet->dst_ip = packet->src_ip;
	packet->dst_port = packet->src_port;		
	
	/* 如果没有其他在线用户 */
	if (p_addr != NULL && p_addr->next == NULL)
	{
		/* 设置要发送的内容 */
		packet->src_ip = -1;
		packet->src_port = -1;

		/* 发送消息 */
		send_data(server_fd, packet); 
		
		return;
	}
	
	/* 发送其他在线用户 */
	while (p_addr != NULL)
	{			
		if (p_addr->ip != packet->dst_ip || 
			p_addr->port != packet->dst_port)
		{
			/* 设置要发送的内容 */
			packet->src_ip = p_addr->ip;
			packet->src_port = p_addr->port;

			/* 发送消息 */
			send_data(server_fd, packet); 
		}
		p_addr = p_addr->next;	
	}
}

/* 增加用户 */
void add_user(int server_fd, addr_t* addr_list,
			  Net_packet* packet)
{
	/* 设置目标地址 */
	packet->dst_ip = packet->src_ip;
	packet->dst_port = packet->src_port;
	
	/* 发送消息 */
	send_data(server_fd, packet);

	/* 增加用户 */
	insert_to_list(addr_list, packet->src_ip, packet->src_port);					 
}

 /* 删除用户 */
void remove_user(int server_fd, addr_t* addr_list,
				 Net_packet* packet)
{	
	remove_from_list(addr_list, packet->src_ip, packet->src_port);	
}

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

