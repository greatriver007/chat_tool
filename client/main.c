#include "udp_client.h"

int main(int argc, char **argv)
{
	int udp_fd = udp_client_init(); // 初始化UDP客户端
	
	/* udp消息发送线程 */
	pthread_t udp_send_tid;
	pthread_create(&udp_send_tid, NULL, udp_send_thread, &udp_fd);

	/* udp消息接收线程 */
	pthread_t udp_recv_tid;
	pthread_create(&udp_recv_tid, NULL, udp_recv_thread, &udp_fd);

	pthread_join(udp_recv_tid, NULL);
	pthread_join(udp_send_tid, NULL);
	
	close(udp_fd);

	return 0;
}

