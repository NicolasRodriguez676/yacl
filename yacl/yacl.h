#ifndef _YACL_H_
#define _YACL_H_

#include <stddef.h>
#include <stdint.h>


//      TYPES

typedef enum yacl_error {
	YACL_SUCCESS        = 0,
	YACL_UNKNOWN_CMD    = 1,
	YACL_NO_CMD         = 2,
	YACL_BUF_FULL       = 3,
	YACL_BUFRS_EMPTD    = 4,

} yacl_error_t;

typedef struct yacl_cmd_cb {
	char* usr_cmd;
	void (* usr_cmd_cb)(uint32_t argc, char** argv, uint32_t* data_out);

} yacl_cmd_cb_t;


//      FUNCTIONS

void yacl_init(yacl_cmd_cb_t* usr_cmd, uint32_t usr_cmd_size, void (* usr_print_func)(char));

yacl_error_t yacl_parse_cmd();
yacl_error_t yacl_wr_buf(char data);

const char* yacl_error_desc(yacl_error_t error);

#endif //_YACL_H_
