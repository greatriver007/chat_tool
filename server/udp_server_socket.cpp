#include "udp_server_socket.h"
#include "def.h"

extern "C"
{
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
}

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <set>

using namespace std;

int create_udp_server_socket(sockaddr_in &addr)
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
	
	/* 绑定套接字地址 */
	if (bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0)
	{
		perror("create_boadcast: bind");
		close(fd);
		exit(0);
	}

	return fd;
}

bool operator<(const sockaddr_in &lhs, const sockaddr_in &rhs)
{
	if (lhs.sin_addr.s_addr == rhs.sin_addr.s_addr)
	{
		return rhs.sin_port < lhs.sin_port;
	}
	return lhs.sin_addr.s_addr < rhs.sin_addr.s_addr;
}

bool operator==(const sockaddr_in &lhs, const sockaddr_in &rhs)
{
	return rhs.sin_port == lhs.sin_port && 
			lhs.sin_addr.s_addr == rhs.sin_addr.s_addr;
}

void udp_server_io(int fd, sockaddr_in &addr)
{
	char msg[200] = {0};
	int msg_len;

	size_t msg_1;
	char msg_2[200] = {0};
	char ip[30];

	/* 用户集合 */
	set<sockaddr_in> addr_set;

	/* 用户地址 */
	sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(sockaddr_in);

	while (1)
	{
		msg_len = recvfrom(fd, msg, sizeof(msg), 0, 
			(sockaddr*)(&client_addr), &client_addr_size);	

		sscanf(msg, "%lu:%s", &msg_1, msg_2);

		if (msg_1 == MSG_ONLINE)
		{	
			/*广播上线通知*/
			for (const auto &addr : addr_set)
			{	
				char msg_send[200];
				sprintf(msg_send, "IP: %s at PORT %u online.\n",
					inet_ntop(AF_INET, &client_addr.sin_addr, ip, 
						sizeof(ip)), ntohs(client_addr.sin_port));

				sendto(fd, msg_send, strlen(msg_send), 0,
					(const sockaddr*)&addr, sizeof(addr));
			}
			/*增加新用户*/
			addr_set.insert(client_addr);
		}	
		else if (msg_1 == MSG_OFFLINE)
		{
			/*删除已下线用户*/
			addr_set.erase(client_addr);
			/*广播下线通知*/
			for (const auto &addr : addr_set)
			{	
				char msg_send[200];
				sprintf(msg_send, "IP: %s at PORT %u offline.\n",
					inet_ntop(AF_INET, &client_addr.sin_addr, ip, 
						sizeof(ip)), ntohs(client_addr.sin_port));

				sendto(fd, msg_send, strlen(msg_send), 0,
					(const sockaddr*)&addr, sizeof(addr));
			}	
		}
		else if (msg_1 == MSG_CHAT)
		{
			printf("%s\n", msg_2);
		}

		memset(msg, 0, sizeof(msg));
		memset(msg_2, 0, sizeof(msg_2));
	}
	
	close(fd);
}
