#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "yacl.h"

#define YACL_LOG
#include "../log.h"

void my_callback(uint32_t argc, char** argv);

void my_print_func(char data)
{
	putchar(data);
}

yacl_cmd_cb_t usr_cmd[] = {
	{ "help", my_callback },
	{ "read", my_callback },
	{ "write", my_callback },
	{ "cmd", my_callback }
};

int main(int argc, char** argv)
{
	char buffer[128];
	char* pbuffer = buffer;

	for (uint32_t i = 1; i < argc; ++i)
	{
		memcpy(pbuffer, argv[i], strlen(argv[i]));
		pbuffer += strlen(argv[i]);
		*(++pbuffer) = ' ';
	}
	*(pbuffer) = '\0';

	yacl_error_t error;
	yacl_init(usr_cmd, sizeof(usr_cmd) / sizeof(usr_cmd[0]), my_print_func);

	for (uint32_t i = 0; i < strlen(buffer); ++i)
	{
		yacl_parse_cmd();
		yacl_wr_buf(buffer[i]);
	}

	yacl_wr_buf('\n');
	error = yacl_parse_cmd();
	LOG_DEBUG("%s\n", yacl_error_desc(error));

	return error;
}

void my_callback(uint32_t argc, char** argv)
{
	LOG_DEBUG("CALLBACK\n");

	char buffer[128];
	char* pbuffer = buffer - 1;

	for (uint32_t i = 0; i < argc; ++i)
	{
		memcpy(++pbuffer, argv[i], strlen(argv[i]));
		pbuffer += strlen(argv[i]);
		*(pbuffer) = ' ';
	}

	*(pbuffer) = '\0';

	LOG_DEBUG("%d - %s\n", argc, buffer);
}
