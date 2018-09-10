#include "header.h"
#include "udp_client.h"
#include "tcp_client.h"

int main(int argc, char **argv)
{
	package_t client_package;	// 通信数据包
	
	udp_client_init(&client_package);	// 初始化UDP客户端
	tcp_client_init(&client_package);	// 初始化TCP客户端

	/* udp消息收发线程 */
	pthread_t udp_client_msg_send_tid;
	pthread_t udp_client_msg_recv_tid;
	pthread_create(&udp_client_msg_send_tid, NULL, 
					udp_client_msg_send_thread, (void*)&client_package);
	pthread_create(&udp_client_msg_recv_tid, NULL, 
					udp_client_msg_recv_thread, (void*)&client_package);

	/* tcp文件收发线程 */
	pthread_t tcp_client_file_send_tid;
	pthread_t tcp_client_file_recv_tid;
	pthread_create(&tcp_client_file_send_tid, NULL, 
					tcp_client_file_send_thread, (void*)&client_package);
	pthread_create(&tcp_client_file_recv_tid, NULL, 
					tcp_client_file_recv_thread, (void*)&client_package);

	pthread_join(udp_client_msg_send_tid, NULL);
	pthread_join(udp_client_msg_recv_tid, NULL);
	pthread_join(tcp_client_file_send_tid, NULL);
	pthread_join(tcp_client_file_recv_tid, NULL);
	
	/* 关闭网络 */
	close(client_package.udp_fd);
	close(client_package.tcp_fd);

	return 0;
}

