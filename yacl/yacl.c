//
// Created by darer on 5/4/2022.
//

#include <string.h>
#include <stdio.h>

#include "yacl.h"

//      TYPES

typedef struct _error_desc {
	const yacl_error_t code;
	const char* msg;

} error_desc_t;

//      GLOBALS

static yacl_cmd_cb_t* _usr_cmd;
static uint32_t _usr_cmd_size;

static char _cmd_bufr[YACL_CMD_LEN_MAX];
static uint8_t _bwx = 0;
static uint8_t _brx = 0;

static uint32_t _buf_status = YACL_PARSE_NOT_RDY;

//      FUNCTION PROTOTYPES

static yacl_error_t _buf_chk();

//      PUBLIC      ****************************************************************************************************

void yacl_init(yacl_cmd_cb_t* usr_cmd, uint32_t usr_cmd_size)
{
	_usr_cmd = usr_cmd;
	_usr_cmd_size = usr_cmd_size;
}

yacl_error_t yacl_wr_buf(char data)
{
	yacl_error_t error = _buf_chk();

	if (error != YACL_SUCCESS)
		return error;

	_cmd_bufr[_bwx & 0x7f] = data;
	++_bwx;

	return YACL_SUCCESS;
}

enum
{
	NO_CB = -1,

	DELIM_SPACE = 32,
	DELIM_NEWLINE = 10
};

void save_token(uint8_t* argc, char** argv, uint8_t* token_begin)
{
	printf("TOKEN -- R%d  B%d  A%d\n", _brx, *token_begin, *argc);

	_cmd_bufr[_brx & 0x7f] = '\0';
	argv[(*argc)++] = _cmd_bufr + *token_begin;      // is argc out of bounds?

	*token_begin = (_brx + 1) & 0x7f;
}

int32_t _get_argv_cb(char** argv)
{
	for (uint32_t i = 0; i < _usr_cmd_size; ++i)
	{
		if (strcmp(argv[0], _usr_cmd[i].usr_cmd) == 0)
			return i;
	}

	return NO_CB;
}

yacl_error_t yacl_parse_cmd()
{
	static uint8_t argc = 0;
	static char* argv[YACL_MAX_ARGS];

	// need a way to remember original begin, if incomplete message is sent
	static uint8_t token_begin = 0;
	int32_t argv_cb;

	printf("PARSE -- R%d  W%d\n", _brx, _bwx);
	while(1)
	{
//		printf("YACL -- R%d  bufr[%c]\n", _brx, _cmd_bufr[_brx & 0x7f]);

		switch (_cmd_bufr[_brx & 0x7f])
		{
		case DELIM_SPACE:
			if (_cmd_bufr[(_brx - 1) & 0x7f] == '\0')
			{
				_cmd_bufr[_brx & 0x7f] = '\0';
				++token_begin;

				break;
			}

			save_token(&argc, argv, &token_begin);
			break;

		case DELIM_NEWLINE:

			if (_cmd_bufr[(_brx - 1) & 0x7f] == '\0')
			{
				printf("NULL -- R%d  bufr[%u]\n", _brx - 1, _cmd_bufr[(_brx - 1) & 0x7f]);

				_cmd_bufr[_brx & 0x7f] = '\0';
				++token_begin;

				break;
			}


			save_token(&argc, argv, &token_begin);
			printf("NEWLINE -- R%d  bufr[%u]\n", _brx, _cmd_bufr[_brx & 0x7f]);

			argv_cb = _get_argv_cb(argv);

			if (argv_cb == NO_CB)
				return YACL_UNKNOWN_CMD;
			else
			{
				_usr_cmd[argv_cb].usr_cmd_cb(argc, argv);
				argc = 0;

				return YACL_SUCCESS;
			}
		}

		if (_brx == (_bwx - 1))
			return YACL_SUCCESS;
		else
			++_brx;
	}
}

void yacl_empty_buf()
{
	_brx = 0;
	_bwx = 0;
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
	if ((_bwx & 0x80) == (_brx & 0x80))
		return YACL_SUCCESS;
	else if ((_bwx | 0x80) == (_brx | 0x80))
		return YACL_BUF_FULL;

	return YACL_SUCCESS;
}
