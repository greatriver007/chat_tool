#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "udp_socket.h"
#include "msg.h"

static void list_online(int fd);	//在线用户列表
static void chat(int fd);			//聊天
static void help();					//提示

static struct sockaddr_in addr;		//套接字地址

static void server_addr_init(struct sockaddr_in* addr) //初始化套接字地址
{
	addr->sin_family = AF_INET;					
	addr->sin_port = htons(PORT);				
	addr->sin_addr.s_addr = htonl(INADDR_ANY);	
}

int udp_init()
{
	/* 创建udp套接字 */
	int fd = socket(PF_INET, SOCK_DGRAM, 0);	
	if (fd < 0)
	{
		perror("create_boadcast: socket");
		exit(1);
	}

	/* 初始化套接字地址 */	
	server_addr_init(&addr);

	/* 重用地址, 允许发送广播数据  */
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0)
	{
		perror("create_boadcast: SO_REUSEADDR");
		close(fd);
		exit(1);
	}
	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) != 0)
	{
		perror("create_boadcast: SO_BROADCAST");
		close(fd);
		exit(1);
	}
	
	return fd;
}

void *udp_send_routine(void *arg)	//消息发送线程函数
{
	int fd = *(int*)arg;
	char cmd[100];
	
	login(fd);
	help();

	while (1)
	{
		/* 获取用户命令 */
		scanf("%100s", cmd);

		/* 解析命令 */
		if (strcmp(cmd, "logout") == 0)
		{
			break;
		}	
		else if (strcmp(cmd, "list") == 0)
		{
			list_online(fd);
		}
		else if (strcmp(cmd, "chat") == 0)
		{
			chat(fd);	
		}
	}

	logout(fd);

	return NULL;
}

void *udp_recv_routine(void *arg)	//消息接受线程函数
{
	int fd = *(int*)arg;
	char msg[200] = {0};
	int msg_len;

	struct sockaddr_in addr;
	socklen_t addr_size = sizeof(addr);

	while(1)
	{
		/* 从服务器接收数据 */
		msg_len = recvfrom(fd, msg, sizeof(msg), 0, 
			(struct sockaddr*)&addr, &addr_size);	
		if (msg_len <= 0)
		{
			continue;
		}
		
		if (strcmp(msg, "logout") == 0)
		{
			break;
		}

		printf("%s\n", msg);

		memset(msg, 0, msg_len);
	}

	return NULL;
}

void login(int fd)
{
	char msg[20];

	/* 发送空消息 */
	sprintf(msg, "%lu:%s", MSG_VOID, "");
	sendto(fd, msg, sizeof(msg), 0, 
		(struct sockaddr*)(&addr), sizeof(addr));

	/* 发送上线通知 */
	sprintf(msg, "%lu:%s", MSG_ONLINE, "");
	sendto(fd, msg, sizeof(msg), 0, 
		(struct sockaddr*)(&addr), sizeof(addr));
}

void logout(int fd)
{
	char msg[20];

	/* 发送下线通知 */
	sprintf(msg, "%lu:%s", MSG_OFFLINE, "");
	sendto(fd, msg, sizeof(msg), 0, 
		(struct sockaddr*)(&addr), sizeof(addr));
}

void list_online(int fd)
{
	char msg[20];

	/* 发送显示在线列表通知 */
	sprintf(msg, "%lu:%s", MSG_LIST, "");
	sendto(fd, msg, sizeof(msg), 0, 
		(struct sockaddr*)(&addr), sizeof(addr));
}

void chat(int fd)
{
	char msg[300];
	char ip[20];
	int ip_num;
	int port;
	char content[200];
	
	/* 设置发送目标 */
	printf("[enter ip and port. example: 192.168.0.1 60000]\n");
	scanf("%s %d", ip, &port);
	getchar();
	inet_pton(AF_INET, ip, &ip_num);

	/* 发送消息 */
	while (1)
	{	
		printf("[enter message to send, or \"stop\" to quit chating]\n");
		fgets(content, 200, stdin);
	
		if (strcmp(content, "stop\n") == 0)
		{
			break;
		}

		sprintf(msg, "%lu:%d:%d:%s", MSG_CHAT, ip_num, port, content);

		sendto(fd, msg, sizeof(msg), 0, 
			(struct sockaddr*)(&addr), sizeof(addr));
		printf("[send succeed]\n");
	}
}

void help()
{
	printf("[enter \"logout\" to logout]\n"
			"[enter \"list\" to list online user]\n"
			"[enter \"chat\" to send message]\n");
}
