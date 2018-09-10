#include "udp_server.h"
#include "addr_list.h"

// 转发消息
static void sendto_target_addr(package_t *package,
				struct sockaddr_in *client_addr,
				const char* msg, int msg_len);

/*发送在线用户地址 */
static void send_online_addr(package_t *package,
				struct sockaddr_in *client_addr, addr_t* addr_list);

/* 初始化TDP客户端 */
int udp_server_init(package_t *package)
{
	/* 设置UDP套接字地址 */
	(package->udp_addr).sin_family = AF_INET;			
	(package->udp_addr).sin_port = htons(UDP_PORT);		
	(package->udp_addr).sin_addr.s_addr = htonl(INADDR_ANY);
	
	/* 创建UDP套接字 */
	package->udp_fd = socket(PF_INET, SOCK_DGRAM, 0);	
	if (package->udp_fd < 0)
	{
		perror("udp_server_init: socket");
		exit(1);
	}
	
	/* 允许重用地址 */ 
	int opt = 1;
	if (setsockopt(package->udp_fd, SOL_SOCKET, SO_REUSEADDR, 
			&opt, sizeof(opt)) != 0)
	{
		perror("udp_server_init: SO_REUSEADDR");
		close(package->udp_fd);
		exit(1);
	}

	/* 允许发送广播数据  */
	if (setsockopt(package->udp_fd, SOL_SOCKET, SO_BROADCAST,
			&opt, sizeof(opt)) != 0)
	{
		perror("udp_server_init: SO_BROADCAST");
		close(package->udp_fd);
		exit(1);
	}
	
	/* 绑定套接字地址 */
	if (bind(package->udp_fd, (struct sockaddr*)&(package->udp_addr), 
		sizeof(package->udp_addr)) != 0)
	{
		perror("udp_server_init: bind");
		close(package->udp_fd);
		exit(1);
	}
}

/* 消息转发线程函数 */
void *udp_server_msg_repost_thread(void *arg)
{
	package_t *package = (package_t *)arg;
	
	char msg[MAX_SIZE];
	int msg_len;

	/* 收到消息的地址 */
	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);

	/* 创建在线用户列表 */
	addr_t* addr_list = create_list();
	
	while (1)
	{
		/* 接收消息 */
		msg_len = recvfrom(package->udp_fd, msg, sizeof(msg), 0, 
							(struct sockaddr*)&client_addr, 
							&client_addr_size);	
		msg[msg_len] = '\0';
		
		/* 解析命令 */
		if (strncmp(msg, MSG_CHAT, MSG_LEN) == 0) 
		{		
			/* 转发消息 */
			sendto_target_addr(package, &client_addr, msg, msg_len);
		}
		else if (strncmp(msg, MSG_ONLINE, MSG_LEN) == 0) 
		{
			/* 发送在线用户地址 */
			send_online_addr(package, &client_addr, addr_list);
		}
		else if (strncmp(msg, MSG_LOGIN, MSG_LEN) == 0) 
		{
			/* 用户上线 */
			insert_list(addr_list, client_addr.sin_addr.s_addr, 
						client_addr.sin_port);
		}	
		else if (strncmp(msg, MSG_LOGOUT, MSG_LEN) == 0) 
		{
			/* 用户下线 */
			remove_from_list(addr_list, client_addr.sin_addr.s_addr, 
								client_addr.sin_port);
			/* 回复下线消息 */
			sendto(package->udp_fd, msg, strlen(msg), 0, 
					(struct sockaddr*)(&client_addr), 
					client_addr_size);
		}
	}

	/* 销毁在线用户列表 */
	destroy_list(&addr_list);
	
	return  NULL;
}

// 转发消息
void sendto_target_addr(package_t *package, 
		struct sockaddr_in *client_addr,
		const char* msg, int msg_len)
{	
	char buf[MAX_SIZE];
	char ip_str[20];
	int ip, port;
	
	/* 设置要发送的内容 */
	sprintf(buf, "%s[收到消息, 来自：IPv4地址: %s 端口号: %d]\n",
			MSG_CHAT,
			inet_ntop(AF_INET, &(client_addr->sin_addr), 
					  ip_str, sizeof(ip_str)),
			ntohs(client_addr->sin_port));

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
	sendto(package->udp_fd, buf, strlen(buf), 0, 
			(struct sockaddr*)(&target_addr), sizeof(target_addr));
}

// 发送在线用户地址
void send_online_addr(package_t *package, 
					  struct sockaddr_in *client_addr, addr_t* addr_list)
{			
	char ip[20];
	char msg[MAX_SIZE];
	
	addr_t *p_addr = addr_list->next;
	
	if (p_addr != NULL && p_addr->next == NULL)
	{
		/* 设置要发送的内容 */
		sprintf(msg, "%s[没有其他在线用户]", MSG_CHAT);
		
		/* 发送消息 */
		sendto(package->udp_fd, msg, strlen(msg), 0, 
					(struct sockaddr*)(client_addr), 
					sizeof(struct sockaddr_in));
		return;
	}
	
	while (p_addr != NULL)
	{			
		/* 如果当前地址和收到消息的地址不同则发送在线用户地址 */
		if (p_addr->ip != (client_addr->sin_addr).s_addr ||
		    p_addr->port != client_addr->sin_port)
		{
			/* 设置要发送的内容 */
			sprintf(msg, "%s[IPv4地址:%s 端口号:%d 在线]",
					MSG_CHAT,
					inet_ntop(AF_INET, &(p_addr->ip), ip, sizeof(ip)),
					ntohs(p_addr->port));
							
			/* 发送消息 */
			sendto(package->udp_fd, msg, strlen(msg), 0, 
					(struct sockaddr*)(client_addr), 
					sizeof(struct sockaddr_in));
		}
		p_addr = p_addr->next;	
	}
}

