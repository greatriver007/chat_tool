#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#define SERVER_IP "192.168.0.103"

#define UDP_PORT 6999
#define TCP_PORT 6999

#define MSG_LEN 	4
#define MSG_VOID 	"VOD:"
#define MSG_ONLINE	"ONL:" 
#define MSG_CHAT	"CHT:" 
#define MSG_LOGIN	"LIN:"
#define MSG_LOGOUT	"LOT:" 	 
#define MSG_FILE 	"FIL:"

#define MSG_CMP(msg, MSG) (strncmp(msg, MSG, MSG_LEN) == 0)

#endif