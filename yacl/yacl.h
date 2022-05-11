//
// Created by darer on 5/4/2022.
//

#ifndef _YACL_H_
#define _YACL_H_

#include <stdint.h>

/*
 *  cmd         - full command typed and passed to parser
 *  user cmd    - single word command that is associated with a user defined callback
 *  args        - arguments or options for given user cmd
 *
 *  newline     - if cmd string contains newline, signal cmd to be parsed
*/

//      GLOBALS


//      TYPES

enum {
	YACL_CMD_LEN_MAX        = 128,
	YACL_MAX_ARGS           = 16,
	YACL_PARSE_RDY          = 0,
	YACL_PARSE_NOT_RDY      = 1,
};

typedef enum yacl_error {
	YACL_SUCCESS = 0,
	YACL_UNKNOWN_CMD = -1,
	YACL_NO_CMD = -2,
	YACL_BUF_FULL = -3,

} yacl_error_t;

typedef struct yacl_cmd_cb {
	char* usr_cmd;
	void (*usr_cmd_cb)(uint32_t argc, char** argv);

} yacl_cmd_cb_t;

//      FUNCTIONS

void yacl_init(yacl_cmd_cb_t* usr_cmd, uint32_t usr_cmd_size);
void yacl_empty_buf();

yacl_error_t yacl_parse_cmd();
yacl_error_t yacl_wr_buf(char data);

const char* yacl_error_desc(yacl_error_t error);

#endif //_YACL_H_
