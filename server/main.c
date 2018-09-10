#include "header.h"
#include "udp_server.h"
#include "tcp_server.h"

int main(int argc, char **argv)
{
	package_t server_package;	// 通信数据包
	
	udp_server_init(&server_package);	// 初始化UDP客户端
	tcp_server_init(&server_package);	// 初始化TCP客户端

	/* udp消息转发线程 */
	pthread_t udp_server_msg_repost_tid;
	pthread_create(&udp_server_msg_repost_tid, NULL, 
					udp_server_msg_repost_thread, (void*)&server_package);

	/* tcp文件收发线程 */
	pthread_t tcp_server_file_send_tid;
	pthread_t tcp_server_file_recv_tid;
	pthread_create(&tcp_server_file_send_tid, NULL, 
					tcp_server_file_send_thread, (void*)&server_package);
	pthread_create(&tcp_server_file_recv_tid, NULL, 
					tcp_server_file_recv_thread, (void*)&server_package);

	pthread_join(udp_server_msg_repost_tid, NULL);
	pthread_join(tcp_server_file_send_tid, NULL);
	pthread_join(tcp_server_file_recv_tid, NULL);
	
	/* 关闭网络 */
	close(server_package.udp_fd);
	close(server_package.tcp_fd);

	return 0;
}

