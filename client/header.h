#ifndef __HEADER_H__
#define __HEADER_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define DEBUG
#define Debug(msg) \
	printf("FILE: %s, LINE: %d, MSG: %s\n", __FILE__, __LINE__, #msg)

#endif
