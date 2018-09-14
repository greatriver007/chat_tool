#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#define SERVER_IP	"192.168.0.102"
#define SERVER_PORT	6999

#define DATA_NONE	0x00
#define DATA_ONLINE 0x01 
#define DATA_CHAT	0x02
#define DATA_LOGIN	0x04
#define DATA_LOGOUT	0x08
#define DATA_FILE 	0x10

#define MAX_DATA_SIZE 256

/* 网络协议包 */
typedef struct NET_PACKET
{
	int src_ip;
	int src_port;
	int dst_ip;
	int dst_port;
	int data_type;
	char data[MAX_DATA_SIZE];
} Net_packet;

#endif