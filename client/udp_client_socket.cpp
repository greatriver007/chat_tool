extern "C"
{
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
}

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "udp_client_socket.h"
#include "def.h"
#include "user_command.h"

static void online_broadcast(int fd, const sockaddr_in& addr);
static void offline_broadcast(int fd, const sockaddr_in& addr);

int create_udp_client_socket(sockaddr_in &addr)
{
	/* 创建套接字 */
	int fd = socket(PF_INET, SOCK_DGRAM, 0);	
	if (fd < 0)
	{
		perror("create_boadcast: socket");
		exit(0);
	}

	/* 设置套接字地址 */	
	addr.sin_family = AF_INET;					
	addr.sin_port = htons(PORT);				
	addr.sin_addr.s_addr = htonl(INADDR_ANY);	

	/* 重用地址, 允许发送广播数据  */
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0)
	{
		perror("create_boadcast: SO_REUSEADDR");
		close(fd);
		exit(0);
	}
	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) != 0)
	{
		perror("create_boadcast: SO_BROADCAST");
		close(fd);
		exit(0);
	}
	
	/* 上线广播 */
	online_broadcast(fd, addr);
	
	return fd;
}

void udp_client_io(int fd, sockaddr_in &addr)
{
	/* 设置IO复用集合 */
	fd_set fds, tfds;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	FD_SET(fd, &fds);
	tfds = fds;
	
	sockaddr_in recvaddr;
	socklen_t recvaddr_size = sizeof(recvaddr);
	
	char msg[200] = {0};
	int msg_len;
	char ip[30];

	/* select IO 复用 */
	while (select(fd + 1,  &tfds, NULL, NULL, NULL) >= 0)
	{
		/* 标准输入 */
		if (FD_ISSET(STDIN_FILENO, &tfds))	
		{
			/* 命令解析 */
			if (user_command(msg, sizeof(msg)) == 0)
			{
				break;
			}

			/* 发送数据到服务器 */
			sendto(fd, msg, strlen(msg), 0, (sockaddr*)&addr, 
				sizeof(addr));			
		}

		/* 套接字 */
		if (FD_ISSET(fd, &tfds))
		{
			/* 从服务器接收数据 */
			msg_len = recvfrom(fd, msg, sizeof(msg), 0, 
				(sockaddr*)&recvaddr, &recvaddr_size);	

		/*	printf("recvfrom: IP: %s at PORT %u:\n",
				inet_ntop(AF_INET, &recvaddr.sin_addr, ip, sizeof(ip)), 
				ntohs(recvaddr.sin_port));*/
			printf("%s\n", msg);

			memset(msg, 0, sizeof(msg));
		}
	
		tfds = fds;
	}
	
	/* 下线广播 */
	offline_broadcast(fd, addr);
}

void online_broadcast(int fd, const sockaddr_in& addr)
{
	char msg[20];

	sprintf(msg, "%lu:%s", MSG_VOID, "");
	sendto(fd, msg, sizeof(msg), 0, (sockaddr*)(&addr), sizeof(addr));

	/* 发送上线通知 */
	sprintf(msg, "%lu:%s", MSG_ONLINE, "");
	sendto(fd, msg, sizeof(msg), 0, (sockaddr*)(&addr), sizeof(addr));
}

void offline_broadcast(int fd, const sockaddr_in& addr)
{
	char msg[20];

	sprintf(msg, "%lu:%s", MSG_VOID, "");
	sendto(fd, msg, sizeof(msg), 0, (sockaddr*)(&addr), sizeof(addr));

	/* 发送下线通知 */
	sprintf(msg, "%lu:%s", MSG_OFFLINE, "");
	sendto(fd, msg, sizeof(msg), 0, (sockaddr*)(&addr), sizeof(addr));
	
	/* 关闭客户端套接字 */
	close(fd);
}
