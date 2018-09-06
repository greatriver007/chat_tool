#include <pthread.h>
#include <unistd.h>

#include "msg.h"
#include "udp_server.h"

int main(int argc, char **argv)
{
	int udp_fd = udp_server_init();	//初始化UDP服务器

	/* 消息收发线程 */
	pthread_t udp_tid;

	pthread_create(&udp_tid, NULL,udp_routine, (void*)&udp_fd);
	
	pthread_join(udp_tid, NULL);

	/* 关闭udp网络 */
	close(udp_fd);

	return 0;
}
