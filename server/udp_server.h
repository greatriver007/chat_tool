#ifndef UDP_SERVER_H
#define UDP_SERVER_H

extern "C"
{
#include <arpa/inet.h>
}

struct sockaddr_in;

int create_udp_server_socket(sockaddr_in &addr);

bool operator<(const sockaddr_in &lhs, const sockaddr_in &rhs);
bool operator==(const sockaddr_in &lhs, const sockaddr_in &rhs);

void udp_server_io(int fd, sockaddr_in &addr);

#endif
