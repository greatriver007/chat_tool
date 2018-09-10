#include "tcp_server.h"

/* 初始化TDP客户端 */
int tcp_server_init(package_t *package)
{
	/* 设置TCP套接字地址 */
	(package->tcp_addr).sin_family = AF_INET;			
	(package->tcp_addr).sin_port = htons(TCP_PORT);		
	(package->tcp_addr).sin_addr.s_addr = htonl(INADDR_ANY);
	
	/* 创建TCP套接字 */
	package->tcp_fd = socket(PF_INET, SOCK_STREAM, 0);	
	if (package->tcp_fd < 0)
	{
		perror("tcp_server_init: socket");
		exit(1);
	}
	
	/* 允许重用地址 */ 
	int opt = 1;
	if (setsockopt(package->udp_fd, SOL_SOCKET, SO_REUSEADDR, 
					&opt, sizeof(opt)) != 0)
	{
		perror("tcp_server_init: setsockopt");
		close(package->udp_fd);
		exit(1);
	}
	
	/* 绑定套接字地址 */
	if (bind(package->tcp_fd, (struct sockaddr*)&(package->tcp_addr), 
				sizeof(package->tcp_addr)) != 0)
	{
		perror("tcp_server_init: bind");
		close(package->tcp_fd);
		exit(1);
	}
	
	/* 最大监听数20 */
	if (listen(package->tcp_fd, 20) == -1)
	{
		perror("tcp_server_init: listen");
		close(package->tcp_fd);
		exit(1);
	}
}

/* 文件发送线程函数 */
void *tcp_server_file_send_thread(void *arg)
{
	package_t *package = (package_t *)arg;

	return NULL;
}

/* 文件接收线程函数 */
void *tcp_server_file_recv_thread(void *arg)
{
	package_t *package = (package_t *)arg;

	return  NULL;
}