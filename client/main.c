#include "udp_client.h"
#include "tcp_client.h"

/* 等待登录成功互斥量和条件变量 */
pthread_mutex_t login_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t login_cond = PTHREAD_COND_INITIALIZER;

int main(int argc, char **argv)
{
	int udp_fd = udp_client_init(); // 初始化UDP客户端

	/* udp消息发送线程 */
	pthread_t udp_send_tid;
	pthread_create(&udp_send_tid, NULL, udp_send_thread, &udp_fd);

	/* 等待登录成功 */
	pthread_mutex_lock(&login_mutex);
	pthread_cond_wait(&login_cond, &login_mutex);
	pthread_mutex_unlock(&login_mutex);
	
	/* udp消息接收线程 */
	pthread_t udp_recv_tid;
	pthread_create(&udp_recv_tid, NULL, udp_recv_thread, &udp_fd);
	
	int tcp_fd = tcp_client_init(); // 初始化TCP客户端
	
	/* tcp文件发送线程 */
	pthread_t tcp_send_tid;
	pthread_create(&tcp_send_tid, NULL, tcp_send_thread, &tcp_fd);
	
	/* tcp文件接收线程 */
	pthread_t tcp_recv_tid;
	pthread_create(&tcp_recv_tid, NULL, tcp_recv_thread, &tcp_fd);

	pthread_join(udp_send_tid, NULL);
	
	close(udp_fd);
	close(tcp_fd);
	
	return 0;
}

