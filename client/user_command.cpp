#include "user_command.h"
#include "def.h"

#include <cstring>
#include <cstdio>

int user_command(char *msg, size_t msg_size)
{
	size_t msg_1 = MSG_VOID;
	char msg_1_str[30];
	char msg_2[200];

	fgets(msg, msg_size, stdin);
	int msg_len = strlen(msg);
	if (msg[msg_len - 2] == '\n')
	{
		msg[msg_len - 2] == '\0';
	}

	/* 字符串转MSG_XXX */
	sscanf(msg, "%[^:]:%s", msg_1_str, msg_2);

	if (strcmp(msg_1_str, "chat") == 0)
	{
		msg_1 = MSG_CHAT;
	}
	else if (strcmp(msg_1_str, "quit") == 0)
	{
		msg_1 = MSG_OFFLINE;
		sprintf(msg, "%lu:%s", msg_1, msg_2);
		return 0;
	}

	sprintf(msg, "%lu:%s", msg_1, msg_2);
	return 1;
}
