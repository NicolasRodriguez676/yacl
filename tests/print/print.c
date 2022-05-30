#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "yacl.h"

#define YACL_LOG
#include "..\log.h"

void my_callback(uint32_t argc, char** argv);

void my_print_func(char data);

yacl_cmd_cb_t usr_cmd[] = {
	{ "help", my_callback },
	{ "read", my_callback },
	{ "write", my_callback },
	{ "cmd", my_callback }
};

int main(int argc, char** argv)
{
	yacl_error_t error;
	yacl_init(usr_cmd, sizeof(usr_cmd) / sizeof(usr_cmd[0]), my_print_func);

	yacl_wr_buf('c');
	yacl_wr_buf('m');
	yacl_wr_buf('d');
	yacl_wr_buf('\n');

	error = yacl_parse_cmd();

	return error;
}

void my_print_func(char data)
{
	putchar(data);
}

void my_callback(uint32_t argc, char** argv) { }