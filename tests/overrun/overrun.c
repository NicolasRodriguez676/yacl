#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "yacl.h"

#define YACL_LOG
#include "log.h"

void my_callback(uint32_t argc, char** argv);

yacl_cmd_cb_t usr_cmd[] = {
	{ "help", my_callback },
	{ "read", my_callback },
	{ "write", my_callback },
	{ "cmd", my_callback }
};

int main(int argc, char** argv)
{
	yacl_error_t error;
	yacl_init(usr_cmd, sizeof(usr_cmd) / sizeof(usr_cmd[0]));

	char cmd[] = "cmd ";
	char arg[] = "arg";
	uint32_t num_args = 13;

	for (uint32_t i = 0; i < strlen(cmd); ++i)
		yacl_wr_buf(cmd[i]);

	for (uint32_t j = 0; j < num_args; ++j)
	{
		for (uint32_t i = 0; i < strlen(arg); ++i)
			error = yacl_wr_buf(arg[i]);

		if ( j != (num_args - 1))
			error = yacl_wr_buf(' ');
	}

	LOG_DEBUG("%s\n", yacl_error_desc(error));

	error = yacl_wr_buf('\n');
	LOG_DEBUG("%s\n", yacl_error_desc(error));

	error = yacl_parse_cmd();
	LOG_DEBUG("%s\n", yacl_error_desc(error));

	yacl_empty_buf();

	for (uint32_t i = 0; i < strlen(cmd); ++i)
		yacl_wr_buf(cmd[i]);

	error = yacl_wr_buf('\n');
	LOG_DEBUG("%s\n", yacl_error_desc(error));

	error = yacl_parse_cmd();
	LOG_DEBUG("%s\n", yacl_error_desc(error));

	return error;
}

void my_callback(uint32_t argc, char** argv)
{
	LOG_DEBUG("CALLBACK\n");
	LOG_DEBUG("%u\n", argc);
}
