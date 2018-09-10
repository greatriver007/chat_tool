#include "udp_client.h"


// 发送消息给UDP服务器 
static void sendto_udp_server(package_t* package, 		
				const void* msg, int msg_len);	
// 从UDP服务器接收消息
static int recvfrom_udp_server(package_t* package, char* msg);	
// 获取用户输入
static int my_gets(char *msg); 							
// 开始聊天
static void start_chat(package_t* package, char* msg);
// 显示帮助
static void show_help();


/* 初始化TDP客户端 */
int udp_client_init(package_t *package)
{
	/* 设置UDP套接字地址 */
	(package->udp_addr).sin_family = AF_INET;			
	(package->udp_addr).sin_port = htons(UDP_PORT);		
	inet_pton(AF_INET, SERVER_IP, &(package->udp_addr).sin_addr);
	
	/* 创建UDP套接字 */
	package->udp_fd = socket(PF_INET, SOCK_DGRAM, 0);	
	if (package->udp_fd < 0)
	{
		perror("udp_client_init: socket");
		exit(1);
	}
	
	/* 允许重用地址 */ 
	int opt = 1;
	if (setsockopt(package->udp_fd, SOL_SOCKET, SO_REUSEADDR, 
					&opt, sizeof(opt)) != 0)
	{
		perror("udp_client_init: SO_REUSEADDR");
		close(package->udp_fd);
		exit(0);
	}

	/* 允许发送广播数据  */
	if (setsockopt(package->udp_fd, SOL_SOCKET, SO_BROADCAST, 
					&opt, sizeof(opt)) != 0)
	{
		perror("udp_client_init: SO_BROADCAST");
		close(package->udp_fd);
		exit(0);
	}
}

/* 消息发送线程函数 */
void *udp_client_msg_send_thread(void *arg)
{
	package_t *package = (package_t *)arg;

	char msg[MAX_SIZE];
	int msg_len;

	/* 发送上线消息给UDP服务器 */
	sendto_udp_server(package, MSG_VOID, MSG_LEN);
	sendto_udp_server(package, MSG_LOGIN, MSG_LEN); 
	
	show_help(); // 显示帮助
	
	while (1)
	{
		msg_len = my_gets(msg);	// 获取用户输入
		
		/* 解析命令 */
		if (strcmp(msg, "logout") == 0)
		{
			break;
		}	
		else if (strcmp(msg, "online") == 0)
		{
			/* 获取在线用户列表 */
			sendto_udp_server(package,  MSG_ONLINE, MSG_LEN); 
		}
		else if (strcmp(msg, "chat") == 0)
		{
			start_chat(package, msg); // 开始聊天
		}
		else if (strcmp(msg, "help") == 0)
		{
			show_help(); // 显示帮助
		}
	}

	/* 发送下线消息给UDP服务器 */
	sendto_udp_server(package,  MSG_LOGOUT, MSG_LEN); 
	
	return NULL;
}

/* 消息接收线程函数 */
void *udp_client_msg_recv_thread(void *arg)
{
	package_t *package = (package_t *)arg;
	
	char msg[MAX_SIZE];
	int msg_len;

	while (1)
	{
		/* 从UDP服务器接收消息 */
		msg_len = recvfrom_udp_server(package, msg); 
		
		/* 解析命令 */
		if (strncmp(msg, MSG_CHAT, MSG_LEN) == 0) 
		{		
			/* 显示收到的消息 */
			printf("%s\n", msg + MSG_LEN);
		}
		if (strncmp(msg, MSG_LOGOUT, MSG_LEN) == 0) 
		{		
			/* 退出线程 */
			break;
		}
	}
	
	return  NULL;
}

/* 获取用户输入 */
int my_gets(char *msg)
{
	if (fgets(msg, MAX_SIZE, stdin) == NULL)
	{
		return 0;
	}
	
	int msg_len = strlen(msg);
	if (msg[msg_len - 1] == '\n')
	{
		msg[msg_len - 1] = '\0';
	}
	--msg_len;
	
	return msg_len;
}

/* 发送消息给UDP服务器 */
void sendto_udp_server(package_t* package, const void* msg, int msg_len)
{
	sendto(package->udp_fd, msg, msg_len, 0, 
			(struct sockaddr*)(&package->udp_addr), 
			sizeof(package->udp_addr));
}

/* 从UDP服务器接收消息 */
int recvfrom_udp_server(package_t* package, char* msg)
{
	struct sockaddr_in addr;
	socklen_t addr_size = sizeof(addr);
	
	int msg_len = recvfrom(package->udp_fd, msg, MAX_SIZE, 0, 
							(struct sockaddr*)&addr, &addr_size);	
	msg[msg_len] = '\0';
	
	return msg_len;
}

/* 开始聊天 */
void start_chat(package_t* package, char* msg)
{
	char ip_str[20];
	int ip;
	int port;
		
	char buf[MAX_SIZE - 12];
	
	/* 输入对方ip */
	printf("[输入对方IPv4地址]\n");
	scanf("%s", ip_str);
	inet_pton(AF_INET, ip_str, &ip);
	
	/* 输入对方端口号 */
	printf("[输入对方端口号]\n");
	scanf("%d", &port);	
	getchar();
	port = htons(port);

	
	while (1)
	{
		/* 输入要发送的内容 */
		printf("[输入要发送的消息/输入quit结束聊天]\n");
		my_gets(buf);
		if (strcmp(buf, "quit") == 0)
		{
			printf("[聊天结束]\n");
			show_help();
			break;
		}
		
		/* 打包消息 */
		sprintf(msg, "%s%d:%d:%s",  MSG_CHAT, ip, port, buf);
		
		sendto_udp_server(package, msg, strlen(msg)); // 发送消息
	}
}
									
/* 显示帮助 */
void show_help()
{
	printf("[输入 online 显示在线用户]\n"
		   "[输入 chat   开始聊天]\n"
		   "[输入 help   获取帮助]\n"
		   "[输入 logout 退出登录]\n");
}
