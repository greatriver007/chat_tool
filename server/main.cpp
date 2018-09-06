#include "udp_server.h"

int main()
{
	sockaddr_in addr;

	int fd = create_udp_server_socket(addr);

	udp_server_io(fd, addr);
}
