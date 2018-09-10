#include "tcp_client.h"

/* 初始化TDP客户端 */
int tcp_client_init(package_t *package)
{
	/* 设置TCP套接字地址 */
	(package->tcp_addr).sin_family = AF_INET;			
	(package->tcp_addr).sin_port = htons(TCP_PORT);		
	(package->tcp_addr).sin_addr.s_addr = htonl(INADDR_ANY);
	
	/* 创建TCP套接字 */
	package->tcp_fd = socket(PF_INET, SOCK_STREAM, 0);	
	if (package->tcp_fd < 0)
	{
		perror("tcp_client_init: socket");
		exit(1);
	}
	
	/* 允许重用地址 */ 
	int opt = 1;
	if (setsockopt(package->tcp_fd, SOL_SOCKET, SO_REUSEADDR, 
					&opt, sizeof(opt)) != 0)
	{
		perror("tcp_client_init: SO_REUSEADDR");
		close(package->tcp_fd);
		exit(0);
	}
}

/* 文件发送线程函数 */
void *tcp_client_file_send_thread(void *arg)
{
	return NULL;
}

/* 文件接收线程函数 */
void *tcp_client_file_recv_thread(void *arg)
{
	return  NULL;
}
