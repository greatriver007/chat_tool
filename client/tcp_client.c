#include "tcp_client.h"

/* 本地ip和端口号 */
extern int LOCAL_IP;
extern int LOCAL_PORT;

/* 初始化TCP客户端 */
int tcp_client_init()
{	
	struct sockaddr_in tcp_addr;

	/* 创建TCP套接字 */
	int tcp_fd = socket(AF_INET, SOCK_STREAM, 0);	
	if (tcp_fd < 0)
	{
		perror("tcp_client_init: 创建UDP套接字失败");
		exit(1);
	}
	
	/* 设置TCP套接字地址 */
	tcp_addr.sin_family = AF_INET;			
	tcp_addr.sin_port = LOCAL_PORT;		
	tcp_addr.sin_addr.s_addr = LOCAL_IP;

	/* 允许重用地址 */ 
	int opt = 1;
	if (setsockopt(tcp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0)
	{
		perror("tcp_client_init: 允许重用地址失败");
		close(tcp_fd);
		exit(1);
	}
	
	/* 绑定套接字地址 */
	if (bind(tcp_fd, (struct sockaddr*)&(tcp_addr), sizeof(tcp_addr)) != 0)
	{
		perror("tcp_server_init: 绑定套接字地址失败");
		close(tcp_fd);
		exit(1);
	}
	
	/* 最大监听数20 */
	if (listen(tcp_fd, 20) == -1)
	{
		perror("tcp_server_init: 设置最大监听数失败");
		close(tcp_fd);
		exit(1);
	}
	
	return tcp_fd;
}
