#include "udp_client_socket.h"

int main()
{
	sockaddr_in addr;

	int fd = create_udp_client_socket(addr); 

	udp_client_io(fd, addr);
}
