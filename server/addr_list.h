#ifndef __ADDR_LIST_H__
#define __ADDR_LIST_H__

typedef struct addr
{
	int ip;
	int port;
	struct addr* next;
} addr_t;

addr_t* create_list();									//创建链表
void insert_to_list(addr_t *head, int ip, int port);	//增加结点
void remove_from_list(addr_t *head, int ip, int port);	//删除结点
void destroy_list(addr_t **p_head);						//销毁链表

#endif
