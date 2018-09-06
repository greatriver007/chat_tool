#include "udp_server.h"
#include "msg.h"

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

static struct sockaddr_in addr;	//	套接字地址

/* 上线广播函数 */
static void online_broadcast(int fd, set<sockaddr_in> &addr_set, 
	const sockaddr_in& client_addr);					

/* 下线广播函数 */
static void offline_broadcast(int fd, set<sockaddr_in> &addr_set, 
	const sockaddr_in& client_addr);					

/* 发送消息函数 */
static void msg_send(int fd, const char *msg, 
	const sockaddr_in& client_addr);					

/* 发送在线列表函数 */
static void online_list_send(int fd, const set<sockaddr_in> &addr_set, 
	const sockaddr_in& client_addr);					

/* 初始化套接字地址函数 */
static void server_addr_init(struct sockaddr_in* addr);	

/*重载sockaddr_in的运算符*/
static bool operator==(const sockaddr_in &lhs, const sockaddr_in &rhs);
static bool operator<(const sockaddr_in &lhs, const sockaddr_in &rhs);


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


void server_addr_init(struct sockaddr_in* addr) 
{
	addr->sin_family = AF_INET;					
	addr->sin_port = htons(PORT);				
	addr->sin_addr.s_addr = htonl(INADDR_ANY);	
}


int udp_server_init()
{
	/* 创建套接字 */
	int fd = socket(PF_INET, SOCK_DGRAM, 0);	
	if (fd < 0)
	{
		perror("create_boadcast: socket");
		exit(0);
	}

	/* 初始化套接字地址 */	
	server_addr_init(&addr);	

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


void *udp_routine(void *arg)	//消息收发线程函数
{
	int fd = *(int*)arg;

	char msg[300];
	int msg_len;
	size_t msg_1;
	char msg_2[200];
	
	set<sockaddr_in> addr_set;	// 用户集合

	/* 用户地址 */
	sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(sockaddr_in);

	while (1)
	{
		/* 从客户端接受数据 */
		msg_len = recvfrom(fd, msg, sizeof(msg), 0, 
			(sockaddr*)(&client_addr), &client_addr_size);	

		sscanf(msg, "%lu:%s", &msg_1, msg_2);
	
		if (msg_1 == MSG_ONLINE)
		{	
			online_broadcast(fd, addr_set, client_addr);	//上线广播
		}	
		else if (msg_1 == MSG_OFFLINE)
		{
			offline_broadcast(fd, addr_set, client_addr);	//下线广播
		}
		else if (msg_1 == MSG_CHAT)
		{
			msg_send(fd, msg_2, client_addr);				//发送消息
		}
		else if (msg_1 == MSG_LIST)
		{	
			online_list_send(fd, addr_set, client_addr);	//发送在线列表
		}
	}
	
	return NULL;
}


void online_broadcast(int fd, set<sockaddr_in> &addr_set, 
	const sockaddr_in& client_addr)
{
	char ip[30];
	char msg_send[200];

	/* 广播上线通知 */
	for (const auto &addr : addr_set)
	{	
		sprintf(msg_send, "[IP: %s at PORT %u login]",
			inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip)), 
			ntohs(client_addr.sin_port));

		sendto(fd, msg_send, strlen(msg_send), 0,
			(const sockaddr*)&addr, sizeof(addr));
	}
	
	/* 增加新用户 */
	addr_set.insert(client_addr);
}


void offline_broadcast(int fd, set<sockaddr_in> &addr_set, 
	const sockaddr_in& client_addr)
{
	char ip[30];
	char msg_send[200];
	
	/* 让用户下线 */
	sprintf(msg_send, "logout");
	sendto(fd, msg_send, strlen(msg_send), 0,
		(const sockaddr*)&client_addr, sizeof(client_addr));

	/* 删除已下线用户 */
	addr_set.erase(client_addr);

	/* 广播下线通知 */
	for (const auto &addr : addr_set)
	{	
		sprintf(msg_send, "[IP: %s at PORT %u logout]",
			inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip)),
			ntohs(client_addr.sin_port));

		sendto(fd, msg_send, strlen(msg_send), 0,
			(const sockaddr*)&addr, sizeof(addr));
	}	
}


void msg_send(int fd, const char *msg, const sockaddr_in& client_addr)
{
	char ip[30];
	int ip_num;
	int port;
	char content[200];

	/* 消息解包 */
	sscanf(msg, "%d:%d:%s", &ip_num, &port, content);
			
	/* 设置目标地址 */
	sockaddr_in _addr;
	_addr.sin_family = AF_INET;					
	_addr.sin_port = htons(port);				
	_addr.sin_addr.s_addr = ip_num;

	/* 消息打包 */
	char msg_send[200];
	sprintf(msg_send, "[message from IP: %s at PORT %u]",
		inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip)), 
		ntohs(client_addr.sin_port));

	/* 发送消息 */
	sendto(fd, msg_send, strlen(msg_send), 0,
		(const sockaddr*)&_addr, sizeof(_addr));
	sendto(fd, content, strlen(content), 0,
		(const sockaddr*)&_addr, sizeof(_addr));
}


void online_list_send(int fd, const set<sockaddr_in> &addr_set, 
	const sockaddr_in& client_addr)
{
	char ip[30];
	char msg_send[200];

	/* 发送在线用户数 */
	sprintf(msg_send, "[total %ld online]", addr_set.size() - 1);

	sendto(fd, msg_send, strlen(msg_send), 0,
		(const sockaddr*)&client_addr, sizeof(client_addr));

	/* 发送在线用户列表 */
	for (const auto &addr : addr_set)
	{	
		if (! (addr == client_addr))
		{	
			sprintf(msg_send, "[IP: %s at PORT %u]",
				inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip)),
				ntohs(addr.sin_port));

			sendto(fd, msg_send, strlen(msg_send), 0,
				(const sockaddr*)&client_addr, sizeof(client_addr));
		}
	}
}
