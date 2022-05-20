#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

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

char* get_rnd_cmd_str()
{
	static char rnd_str[128];
	uint32_t rnd_str_idx;

	uint32_t num_usr_cmds = sizeof(usr_cmd) / sizeof(usr_cmd[0]);
	uint32_t rnd_cmd_idx = (rand() % num_usr_cmds);

	rnd_str_idx = strlen(usr_cmd[rnd_cmd_idx].usr_cmd);
	memcpy(rnd_str, usr_cmd[rnd_cmd_idx].usr_cmd, rnd_str_idx);

	char arg[] = "arg";

	srand(time(0));
	uint32_t rnd_num_args = (rand() % 12) + 1;

	if (rnd_num_args == 1)
	{
		rnd_str[rnd_str_idx] = '\n';
		return rnd_str;
	}
	else
		rnd_str[rnd_str_idx++] = ' ';

	for (uint32_t i = 1; i < rnd_num_args; ++i)
	{
		for (uint32_t j = 0; j < strlen(arg); ++j)
			rnd_str[rnd_str_idx++] = arg[j];

		rnd_str_idx += snprintf(rnd_str + rnd_str_idx, 128 - rnd_str_idx, "%u ", i);
	}
	rnd_str[rnd_str_idx - 1] = '\n';
	rnd_str[rnd_str_idx] = '\0';

	return rnd_str;
}

int main(int argc, char** argv)
{
	yacl_error_t error;
	yacl_init(usr_cmd, sizeof(usr_cmd) / sizeof(usr_cmd[0]));

	char* cmd;

	for (uint32_t j = 0; j < 100000; ++j)
	{
		srand(time(0));

		cmd = get_rnd_cmd_str();

		for (uint32_t i = 0; i < strlen(cmd); ++i)
			yacl_wr_buf(cmd[i]);

		error = yacl_parse_cmd();
	}
	LOG_DEBUG("%s\n\n", yacl_error_desc(error));

	return error;
}

void my_callback(uint32_t argc, char** argv) { }