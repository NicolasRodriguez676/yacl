#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "yacl.h"


void my_callback(uint32_t argc, char** argv);

yacl_cmd_cb_t usr_cmd[] = {
	{ "help", my_callback },
	{ "read", my_callback },
	{ "write", my_callback }
};

int main(int argc, char** argv)
{
	for (uint32_t i = 1; i < argc; ++i)
		printf("%s", argv[i]);

	return 0;
}

void my_callback(uint32_t argc, char** argv)
{
	for (uint32_t i = 0; i < (argc - 1); ++i)
		printf("%s\n", argv[i]);
}
