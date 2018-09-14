#ifndef __ADDR_LIST_H__
#define __ADDR_LIST_H__

/* 结点定义 */
typedef struct addr
{
	int ip;
	int port;
	struct addr* next;
} addr_t;

/* 创建链表 */
addr_t* create_list();			
		
/* 增加结点 */				
void insert_to_list(addr_t *head, int ip, int port);

/* 删除结点 */
void remove_from_list(addr_t *head, int ip, int port);	

/* 销毁链表 */
void destroy_list(addr_t **p_head);						

#endif
