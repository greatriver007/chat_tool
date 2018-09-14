#include "tcp_client.h"

/* 本地ip和端口号 */
extern int LOCAL_IP;
extern int LOCAL_PORT;

static void* file_recv_thread(void* arg); // 文件接收子线程函数
static void recv_filename(int src_fd, char *filename); // 接收发送者和文件名函数

/* 文件接收线程 */
void* tcp_recv_thread(void *p_tcp_fd)
{	
	pthread_detach(pthread_self());	//设置分离属性
	
	int tcp_fd = *(int*)p_tcp_fd;
	
	struct sockaddr_in tcp_addr;
	
	/* 设置TCP套接字地址 */
	tcp_addr.sin_family = AF_INET;			
	tcp_addr.sin_port = LOCAL_PORT;		
	tcp_addr.sin_addr.s_addr = LOCAL_IP;
	
	socklen_t socklen = sizeof(tcp_addr);
	
	while(1)
	{
		int *src_fd = (int*)malloc(sizeof(int));
		if (src_fd == NULL)
		{
			perror("tcp_recv_thread: malloc");
			return NULL;
		}

		/* 等待连接 */
		*src_fd = accept(tcp_fd, (struct sockaddr*)&tcp_addr, &socklen);
		if (*src_fd < 0)
		{
			perror("tcp_recv_thread: 等待连接失败");
			close(*src_fd);
			return NULL;
		}

		/* 文件接收子线程函数 */
		pthread_t file_recv_tid;
		pthread_create(&file_recv_tid, NULL, file_recv_thread, src_fd);
	}
	
	return NULL;
}

/* 文件接收子线程函数 */
void* file_recv_thread(void* arg)
{
	pthread_detach(pthread_self());	//设置分离属性

	int src_fd = *(int*)arg;

	char filename[MAX_DATA_SIZE];
	
	/* 接收发送者和文件名 */
	recv_filename(src_fd, filename);

	/* 创建文件 */
	int fd = open(filename, O_WRONLY | O_CREAT, 0666);
	if (fd < 0)
	{
		perror("file_recv_thread: 创建文件失败");
		return NULL;
	}
	
	/*接收文件*/
	char buf[1024];
	int buf_len;
	while ((buf_len = recv(src_fd, buf, sizeof(buf), 0)) > 0)
	{
		write(fd, buf, buf_len);
	}
	
	/* 关闭文件 */
	close(fd);
	/* 关闭连接 */
	close(src_fd);
	
	return NULL;
}

void recv_filename(int src_fd, char *filename)
{
	Net_packet packet;
	
	/* 接收数据 */
	recv(src_fd, &packet, sizeof(packet), 0); 

	/* 获取目前时间 */
    time_t t;
    struct tm *pt;
    time(&t);
    pt = gmtime(&t);
	
	/* 打印发送者和文件名 */
	char ip_str[20];
	printf("[收到文件! %d:%d:%d]\n"
		   "[文件名：%s]\n"
		   "[来自用户: IP:%s 端口:%d]\n",
		   8 + pt->tm_hour,pt->tm_min, pt->tm_sec, 
		   packet.data,
		   inet_ntop(AF_INET, &packet.src_ip, ip_str, sizeof(ip_str)),
		   ntohs(packet.src_port));
	  
	stpcpy(filename, "./file/");
	strcat(filename, packet.data);
}