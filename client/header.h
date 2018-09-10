#ifndef __HEADER_H__
#define __HEADER_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define SERVER_IP "192.168.0.106"

#define UDP_PORT 6999
#define TCP_PORT 6999

#define MAX_SIZE 256

#define MSG_LEN 	4
#define MSG_VOID 	"VOD:"
#define MSG_ONLINE	"ONL:" 
#define MSG_CHAT	"CHT:" 
#define MSG_LOGIN	"LIN:"
#define MSG_LOGOUT	"LOT:" 	 

#define DEBUG
#define Debug(msg) \
	printf("FILE: %s, LINE: %d, MSG: %s\n", __FILE__, __LINE__, #msg)

typedef struct Package
{
	int udp_fd;
	int tcp_fd;
	struct sockaddr_in udp_addr;
	struct sockaddr_in tcp_addr;
} package_t;

#endif
