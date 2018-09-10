#include "addr_list.h"

addr_t* create_list()
{
	addr_t *head = (addr_t*)malloc(sizeof(addr_t));
	if(head == NULL)
	{
		perror("create_list: malloc");
	}

	head->next = NULL;

	return head;
}

addr_t* find_in_list(addr_t *head, int ip, int port)
{
	addr_t *p = head->next;

	while ((p != NULL) && ((p->ip != ip) || (p->port != port)))
	{
		p = p->next;
	}
	
	return p;
}

void insert_list(addr_t *head, int ip, int port)
{
	if (find_in_list(head, ip, port) != NULL)
	{
		return;
	}
	
	addr_t *p_new = (addr_t*)malloc(sizeof(addr_t));
	if(p_new == NULL)
	{
		perror("append_list: malloc");
	}

	p_new->ip = ip;
	p_new->port = port;
	p_new->next = head->next;

	head->next = p_new;
}

void remove_from_list(addr_t *head, int ip, int port)
{
	addr_t *p = head;

	while ((p->next != NULL) && 
		((p->next->ip != ip) || (p->next->port != port)))
	{
		p = p->next;
	} 
	
	if (p->next == NULL)
	{
		return;
	}

	addr_t *tmp = p->next;
	
	p->next = p->next->next;
	
	free(tmp);
}

void clear_list(addr_t *head)
{
	addr_t *p = head->next;

	while (p != NULL)
	{
		head->next = p->next;
		free(p);
		p = head->next;
	}
}

void destroy_list(addr_t **p_head)
{
	clear_list(*p_head);
	free(*p_head);
	*p_head = NULL;
}

void display_list(addr_t *head)
{
	addr_t *p = head->next;

	while (p != NULL)
	{
		printf("%d %d\n", p->ip, p->port);	
		p = p->next;	
	}
}
