#include "tcp_client.h"

/* 本地ip和端口号 */
extern int LOCAL_IP;
extern int LOCAL_PORT;

/* 等待文件发送操作结束的互斥量和条件变量 */
extern pthread_cond_t chat_cond;

/* 等待文件发送开始的互斥量和条件变量 */
pthread_mutex_t file_send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t file_send_cond = PTHREAD_COND_INITIALIZER;

/* 文件发送子线程函数 */
static void* file_send_thread(void* arg);	

/* 建立tcp连接 */
static int tcp_connect(); 				
	
/* 读取要上传的文件路径 */
static void get_path(char *path); 			


/* 文件发送线程 */
void* tcp_send_thread(void *tcp_fd)
{
	pthread_detach(pthread_self());	//设置分离属性
	
	while (1)
	{
		/* 等待文件发送信号 */
		pthread_mutex_lock(&file_send_mutex);
		pthread_cond_wait(&file_send_cond, &file_send_mutex);	
		pthread_mutex_unlock(&file_send_mutex);	
		
		/* 文件发送子线程函数 */
		pthread_t file_send_tid;
		if (pthread_create(&file_send_tid, NULL, file_send_thread, tcp_fd) 
			!= 0)
		{
#ifdef DEBUG
			perror("tcp_send_thread: 创建文件发送子线程失败");
#endif
			return NULL;		
		}	
	}
	return NULL;
}


/* 文件发送子线程函数 */
void* file_send_thread(void* arg)
{
	pthread_detach(pthread_self());	//设置分离属性

	int dst_fd = tcp_connect(); //建立tcp连接
		
	/* 读取要上传的文件路径 */
	char path[MAX_DATA_SIZE];
	get_path(path);

	/* 只读打开文件 */
	int fd = open(path, O_RDONLY);
	if (fd < 0)
	{
#ifdef DEBUG
		perror("file_send_thread: 只读打开文件失败");
#endif
		pthread_cond_signal(&chat_cond); // 唤醒聊天线程
		return NULL;
	}
	
	/* 设置发送者 */
	Net_packet packet;
	packet.src_ip = LOCAL_IP;
	packet.src_port = LOCAL_PORT;
	
	/* 读取要上传的文件名 */
	printf("[输入要上传的文件名]\n");
	scanf("%s", packet.data);
	getchar();

	pthread_cond_signal(&chat_cond); // 唤醒聊天线程

	/* 发送文件名 */
	send(dst_fd, &packet, sizeof(packet), 0);
	
	/* 发送文件 */
	char buf[1024];
	int buf_len;
	while ((buf_len = read(fd, buf, sizeof(buf))) > 0)
	{
		send(dst_fd, buf, buf_len, 0);
	}
	
	/* 关闭文件 */
	close(fd);
	
	return NULL;
}


/* 建立tcp连接 */
int tcp_connect()
{
	char ip_str[20];
	int port;
	
	/* 输入对方ip */
	printf("[输入对方IPv4地址]\n");
	scanf("%s", ip_str);
	getchar();
	
	/* 输入对方端口号 */
	printf("[输入对方端口号]\n");
	scanf("%d", &port);	
	getchar();
	
	/* 设置TCP套接字地址 */
	struct sockaddr_in dst_addr;
	dst_addr.sin_family = AF_INET;			
	dst_addr.sin_port = htons(port);	
	inet_pton(AF_INET, ip_str, &dst_addr.sin_addr);	
	
	/* 创建TCP套接字 */
	int dst_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (dst_fd < 0)
	{
#ifdef DEBUG
		perror("file_send_thread: 创建TCP套接字失败");
#endif
		pthread_cond_signal(&chat_cond); // 唤醒聊天线程
		return -1;
	}
	
	/* 建立连接 */
	if (connect(dst_fd, (struct sockaddr*)&(dst_addr), 
				sizeof(dst_addr)) != 0)
	{
#ifdef DEBUG
		perror("file_send_thread: 建立TCP连接失败");
#endif
		close(dst_fd);
		pthread_cond_signal(&chat_cond); // 唤醒聊天线程
		return -1;
	}
	
	return dst_fd;
}


/* 读取要上传的文件路径 */
void get_path(char *path)
{
	printf("[输入要上传的文件路径]\n");
	while (1)
	{
		scanf("%s", path);
		getchar();
	
		/* 测试能否打开文件并且可读 */
		if (access(path, F_OK) == 0 && access(path, R_OK) == 0)
		{
			break;
		}		
		printf("[无效的文件路径, 请重新输入]\n");
	}
}












