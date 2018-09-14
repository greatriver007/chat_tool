#include "udp_client.h"

/* 唤醒文件发送的条件变量 */
extern pthread_cond_t file_send_cond;

/* 等待文件发送操作结束的互斥量和条件变量 */
pthread_mutex_t chat_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t chat_cond = PTHREAD_COND_INITIALIZER;

/* 本地ip和端口号 */
int LOCAL_IP = 0;
int LOCAL_PORT = 0;

/* 登录状态条件变量 */
extern pthread_cond_t login_cond;

/* 获取指令 */
static int get_data_type();

/* 登录 */
static void login(int udp_fd, Net_packet* packet);

/* 退出登录 */
static void logout(int udp_fd, Net_packet* packet);

/* 发送消息 */
static void start_chat(int udp_fd, Net_packet* packet);

/* 获取在线用户 */
static void get_onlines(int udp_fd, Net_packet* packet);

/* 显示帮助 */
static void show_help();

/* 获取用户输入 */
static int get_msg(char *msg, int msg_len);


/* 数据发送线程 */
void* udp_send_thread(void *p_udp_fd)
{
	int udp_fd = *(int*)p_udp_fd;

	Net_packet packet;
	
	login(udp_fd, &packet);	// 登录

	while(1)
	{
		packet.data_type = get_data_type();	//获取指令
		
		/* 解析指令 */
		switch(packet.data_type)
		{
		case DATA_CHAT:  // 发送消息
			start_chat(udp_fd, &packet);
			break;
		case DATA_ONLINE: // 获取在线用户
			send_data(udp_fd, &packet);
			break;
		case DATA_LOGOUT: // 退出登录
			logout(udp_fd, &packet);
			return NULL;
		case DATA_FILE:
		
			pthread_cond_signal(&file_send_cond); //唤醒文件发送线程
			
			/* 等待文件发送操作接收 */
			pthread_mutex_lock(&chat_mutex);
			pthread_cond_wait(&chat_cond, &chat_mutex);	
			pthread_mutex_unlock(&chat_mutex);	
			
			break;
		default :
			show_help();
		}
	}

	return NULL;
}


/* 获取指令 */
int get_data_type()
{
	char cmd[100];
	
	/* 输入指令 */
	scanf("%s", cmd);
	getchar();
	
	/* 解析指令*/
	if (strcmp(cmd, "logout") == 0)
	{
		return DATA_LOGOUT;
	}	
	if (strcmp(cmd, "chat") == 0)
	{
		return DATA_CHAT;
	}		
	if (strcmp(cmd, "online") == 0)
	{
		return DATA_ONLINE;
	}	
	if (strcmp(cmd, "file") == 0)
	{
		return DATA_FILE;
	}
	return -1;
}


/* 登录 */
void login(int udp_fd, Net_packet* packet)
{
	packet->data_type = DATA_NONE;
	send_data(udp_fd, packet);	
	
	/* 请求登录 */
	packet->data_type = DATA_LOGIN;
	send_data(udp_fd, packet);	
	
	/* 接收回执 */
	recv_data(udp_fd, packet);	
	LOCAL_IP = packet->src_ip;
	LOCAL_PORT = packet->src_port;
	
	/* 登录成功 */
	pthread_cond_signal(&login_cond);
		
	/* 打印本机用户地址 */
	char ip_str[20];
	printf("[本地用户: IP:%s 端口:%d]\n",
		   inet_ntop(AF_INET, &LOCAL_IP, ip_str, sizeof(ip_str)),
		   ntohs(LOCAL_PORT));
	
	/* 打印帮助 */
	show_help();
}


/* 退出登录 */
void logout(int udp_fd, Net_packet* packet)
{
	packet->data_type = DATA_LOGOUT;
	send_data(udp_fd, packet);	
}


/* 发送消息 */
void start_chat(int udp_fd, Net_packet* packet)
{
	char ip[20];

	/* 设置目标ip */
	printf("[请输入对方IPv4地址, 如\"0.0.0.0\"]\n");
	scanf("%s", ip);
	getchar();
	inet_pton(AF_INET, ip, &packet->dst_ip);
	
	/* 设置目标端口号 */
	printf("[请输入对方端口号, 如\"10000\"]\n");
	scanf("%d", &packet->dst_port);	
	getchar();
	packet->dst_port = htons(packet->dst_port);
	
	printf("[开始聊天]\n");
	while (1)
	{
		/* 输入要发送的内容 */
		printf("[请输入要发送的消息, 或者输入quit结束聊天]\n");
		get_msg(packet->data, MAX_DATA_SIZE);
		
		/* 结束聊天 */
		if (strcmp(packet->data, "quit") == 0)
		{
			printf("[聊天结束]\n");
			break;
		}
		
		/* 发送数据 */
		send_data(udp_fd, packet);
	}
}


/* 获取在线用户 */
void get_onlines(int udp_fd, Net_packet* packet)
{
	packet->data_type = DATA_ONLINE;
	send_data(udp_fd, packet);
	printf("1111\n");
}


/* 显示帮助 */
void show_help()
{
	printf("[输入 online 显示在线用户]\n"
		   "[输入 chat   开始聊天]\n"
		   "[输入 file   发送文件]\n"
		   "[输入 help   获取帮助]\n"
		   "[输入 logout 退出登录]\n");
}


/* 获取用户输入 */
int get_msg(char *msg, int msg_len)
{
	if (fgets(msg, msg_len, stdin) == NULL)
	{
		return 0;
	}
	
	int len = strlen(msg);
	if (msg[len - 1] == '\n')
	{
		msg[len - 1] = '\0';
	}
	--len;
	
	return len;
}