#ifndef __ADDR_LIST_H__
#define __ADDR_LIST_H__

#include "header.h"

typedef struct addr
{
	int ip;
	int port;
	struct addr* next;
} addr_t;

addr_t* create_list();
addr_t* find_in_list(addr_t *head, int ip, int port);
void insert_list(addr_t *head, int ip, int port);
void remove_from_list(addr_t *head, int ip, int port);
void clear_list(addr_t *head);
void destroy_list(addr_t **p_head);
void display_list(addr_t *head);

#endif
