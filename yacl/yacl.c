//
// Created by darer on 5/4/2022.
//

#include <string.h>

#include "yacl.h"

//      TYPES

typedef struct _error_desc {
	const yacl_error_t code;
	const char* msg;

} error_desc_t;

typedef struct _cmd_buf {
	char* bufr;

	uint8_t rdx;
	uint8_t wdx;

} yacl_cmd_buf_t;

//      GLOBALS

static const char _delim_space[] = " ";
static const char _cmd_entered_char[] = "\n";

static yacl_cmd_cb_t* _usr_cmd;
static uint32_t _usr_cmd_size;

static yacl_cmd_buf_t _cmd_buf;
static uint32_t _buf_status;

//      FUNCTION PROTOTYPES

static yacl_error_t _buf_chk();

//      PUBLIC      ****************************************************************************************************

void yacl_init(yacl_cmd_cb_t* usr_cmd, uint32_t usr_cmd_size)
{
	static char cmd_buf[YACL_CMD_LEN_MAX];

	_cmd_buf.bufr = cmd_buf;
	_cmd_buf.rdx = 0;
	_cmd_buf.wdx = 0;

	_usr_cmd = usr_cmd;
	_usr_cmd_size = usr_cmd_size;

	_buf_status = YACL_PARSE_NOT_RDY;
}

yacl_error_t yacl_wr_buf(char data)
{
	yacl_error_t error = _buf_chk();

	if (error != YACL_SUCCESS)
		return error;

	if (data == _cmd_entered_char[0])
		_buf_status = YACL_PARSE_RDY;

	_cmd_buf.bufr[_cmd_buf.wdx & 0x7f] = data;
	++(_cmd_buf.wdx);

	return YACL_SUCCESS;
}

yacl_error_t yacl_parse_cmd()
{
	if (_buf_status != YACL_PARSE_RDY)
		return YACL_NO_CMD;

	_buf_status = YACL_PARSE_NOT_RDY;

	char* token = strtok(_cmd_buf.bufr, _delim_space);
	char* argv[YACL_MAX_ARGS + 1];

	argv[0] = token;
	_cmd_buf.rdx += strlen(token + 1);

	uint32_t usr_cmd_idx = 0;

	while (strcmp(argv[0], _usr_cmd[usr_cmd_idx].usr_cmd) != 0)
	{
		++usr_cmd_idx;

		if (usr_cmd_idx == _usr_cmd_size)
			return YACL_UNKNOWN_CMD;
	}

	uint32_t argc = 1;

	while (1)
	{
		token = strtok(NULL, _delim_space);

		if (token == NULL)
			break;

		_cmd_buf.rdx += strlen(token) + 1;

		argv[argc] = token;
		++argc;
	}

	_usr_cmd[usr_cmd_idx].usr_cmd_cb(argc, argv);

	return YACL_SUCCESS;
}

void yacl_empty_buf()
{
	_cmd_buf.rdx = 0;
	_cmd_buf.wdx = 0;
}

const char* yacl_error_desc(yacl_error_t error)
{
	static const error_desc_t error_desc[] = {
		{ YACL_SUCCESS, "command successfully executed" },
		{ YACL_UNKNOWN_CMD, "command was not found in given commands" },
		{ YACL_NO_CMD, "command incomplete" },
		{ YACL_BUF_FULL, "buffer full. try emptying buffer" }
	};

	if (error <= 0)
		return error_desc[error * -1].msg;
	else
		return NULL;
}

//      PRIVATE     ****************************************************************************************************

static yacl_error_t _buf_chk()
{
	if ((_cmd_buf.wdx & 0x80) == (_cmd_buf.rdx & 0x80))
		return YACL_SUCCESS;
	else
		if ((_cmd_buf.wdx | 0x80) == (_cmd_buf.rdx | 0x80))
			return YACL_BUF_FULL;

	return YACL_SUCCESS;
}
