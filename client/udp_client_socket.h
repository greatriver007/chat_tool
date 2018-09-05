#ifndef UDP_CLIENT_SOCKET_H
#define UDP_CLIENT_SOCKET_H

extern "C"
{
#include <arpa/inet.h>
}

int create_udp_client_socket(sockaddr_in &addr);
void udp_client_io(int fd, sockaddr_in &addr);

#endif
