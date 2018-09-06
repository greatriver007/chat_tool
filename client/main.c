#include <pthread.h>
#include <unistd.h>

#include "msg.h"
#include "udp_socket.h"

int main(int argc, char **argv)
{
	int udp_fd = udp_init();	//初始化UDP网络

	/* 消息收发线程 */
	pthread_t udp_send_tid;
	pthread_t udp_recv_tid;

	pthread_create(&udp_send_tid, NULL,udp_send_routine, (void*)&udp_fd);
	pthread_create(&udp_recv_tid, NULL, udp_recv_routine, (void*)&udp_fd);

	pthread_join(udp_send_tid, NULL);
	pthread_join(udp_recv_tid, NULL);


	/* 关闭udp网络 */
	close(udp_fd);

	return 0;
}
