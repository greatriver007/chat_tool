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

#define MAX_SIZE 256

#define DEBUG
#define Debug(msg) \
	printf("FILE: %s, LINE: %d, MSG: %s\n", __FILE__, __LINE__, #msg)

/* 初始化TDP客户端 */
static int server_init(struct sockaddr_in* server_addr);

/* 数据接收函数 */
static int recv_msg(int server_fd, char* msg, int msg_len, 
					 struct sockaddr_in *client_addr);

/* 消息转发函数 */
static void send_msg(int server_fd, const char* msg, int msg_len,
					 const struct sockaddr_in *client_addr);
				
/* 发送在线用户函数 */
static void send_onlines(int server_fd, const addr_t* addr_list,
						 const struct sockaddr_in *client_addr);

/* 服务器例程 */
void server_routine()
{
	char msg[MAX_SIZE];
	int msg_len;
	struct sockaddr_in server_addr, client_addr;

	int server_fd = server_init(&server_addr); // 初始化服务器
	
	addr_t* addr_list = create_list(); // 创建在线用户列表
	
	while (1)
	{
		/* 接收数据 */
		msg_len = recv_msg(server_fd, msg, MAX_SIZE, &client_addr);
		
		/* 解析命令 */
		if (MSG_CMP(msg, MSG_CHAT)) // 转发消息
		{		
			send_msg(server_fd, msg, msg_len, &client_addr);
		}
		else if (MSG_CMP(msg, MSG_ONLINE)) // 发送在线用户
		{
			send_onlines(server_fd, addr_list, &client_addr);
		}
		else if (MSG_CMP(msg, MSG_LOGIN)) // 增加用户
		{
			insert_to_list(addr_list, 
						   (int)client_addr.sin_addr.s_addr, 
						   (int)client_addr.sin_port);
		}	
		else if (MSG_CMP(msg, MSG_LOGOUT)) // 删除用户
		{
			remove_from_list(addr_list, 
			                 (int)client_addr.sin_addr.s_addr, 
							 (int)client_addr.sin_port);

			sendto(server_fd, msg, strlen(msg), 0, 
				   (struct sockaddr*)(&client_addr), 
				   sizeof(client_addr));
		}
	}

	destroy_list(&addr_list); // 销毁在线用户列表
		
	close(server_fd); // 关闭服务器
}


/* 初始化TDP客户端 */
static int server_init(struct sockaddr_in* server_addr)
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
	server_addr->sin_port = htons(UDP_PORT);		
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


/* 数据接收函数 */
int recv_msg(int server_fd, char* msg, int msg_len,
			  struct sockaddr_in *client_addr)
{
	socklen_t client_addr_size = sizeof(client_addr);
	
	int len = recvfrom(server_fd, msg, msg_len, 0, 
						   (struct sockaddr*)client_addr, 
						   &client_addr_size);
	msg[len] = '\0';
	
	return len;
}


/* 消息转发函数 */
void send_msg(int server_fd, const char* msg, int msg_len,
			  const struct sockaddr_in *client_addr)
{	
	char buf[MAX_SIZE];
	char ip_str[20];
	int ip, port;
	
	/* 设置要发送的内容 */
	inet_ntop(AF_INET, &(client_addr->sin_addr), ip_str, sizeof(ip_str));
	port = ntohs(client_addr->sin_port);
	sprintf(buf, "%s[收到消息, 来自：IPv4地址: %s 端口号: %d]\n",
			MSG_CHAT, ip_str, port);
	sscanf(msg + MSG_LEN, "%d:%d", &ip, &port);
	const char* p_msg = msg + MSG_LEN;
	while (*p_msg++ != ':');
	while (*p_msg++ != ':');
	strcat(buf, p_msg);
	
	/* 设置目标地址 */
	struct sockaddr_in target_addr;	
	target_addr.sin_family = AF_INET;
	target_addr.sin_port = port;		
	target_addr.sin_addr.s_addr = ip;
		
	/* 发送消息 */
	sendto(server_fd, buf, strlen(buf), 0, 
		   (struct sockaddr*)(&target_addr), sizeof(target_addr));
}


/* 发送在线用户函数 */
void send_onlines(int server_fd, const addr_t* addr_list,
				  const struct sockaddr_in *client_addr)
{			
	char ip[20];
	char msg[MAX_SIZE];
	
	const addr_t *p_addr = addr_list->next;
	
	/* 如果没有其他在线用户 */
	if (p_addr != NULL && p_addr->next == NULL)
	{
		/* 设置要发送的内容 */
		sprintf(msg, "%s[没有其他在线用户]", MSG_CHAT);
		
		/* 发送消息 */
		sendto(server_fd, msg, strlen(msg), 0, 
			   (struct sockaddr*)(client_addr), 
			   sizeof(struct sockaddr_in));
			   
		return;
	}
	
	/* 发送其他在线用户 */
	while (p_addr != NULL)
	{			
		if (p_addr->ip != (client_addr->sin_addr).s_addr ||
		    p_addr->port != client_addr->sin_port)
		{
			/* 设置要发送的内容 */
			sprintf(msg, "%s[IPv4地址:%s 端口号:%d 在线]",
					MSG_CHAT,
					inet_ntop(AF_INET, &(p_addr->ip), ip, sizeof(ip)),
					ntohs(p_addr->port));
							
			/* 发送消息 */
			sendto(server_fd, msg, strlen(msg), 0, 
				   (struct sockaddr*)(client_addr), 
				   sizeof(struct sockaddr_in));
		}
		p_addr = p_addr->next;	
	}
}

