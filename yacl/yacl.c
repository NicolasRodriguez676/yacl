#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "yacl.h"

#define YACL_LOG
#include "log.h"

//      TYPES

enum
{
	NO_CB               = -1,

	DELIM_SPACE         = 32,
	DELIM_NEWLINE       = 10,

	YACL_CMD_LEN_MAX    = 128,
	YACL_MAX_ARGS       = 12
};

typedef struct _error_desc {
	const yacl_error_t code;
	const char* msg;

} error_desc_t;

//      GLOBALS

static yacl_cmd_cb_t* _usr_cmd;
static uint32_t _usr_cmd_size;

static char _cmd_bufr[YACL_CMD_LEN_MAX];
static uint8_t _brx = 0;
static uint8_t _bwx = 0;

static uint8_t _argv_token_zero = 0;

//      FUNCTION PROTOTYPES

static yacl_error_t _buf_chk();
static void _save_token(uint8_t argc, char** argv, uint8_t* token_begin);
static int32_t _get_argv_cb();

//      PUBLIC      ****************************************************************************************************

void yacl_init(yacl_cmd_cb_t* usr_cmd, uint32_t usr_cmd_size)
{
	_usr_cmd = usr_cmd;
	_usr_cmd_size = usr_cmd_size;
}

yacl_error_t yacl_wr_buf(char data)
{
	if (_buf_chk() != YACL_SUCCESS)
		return YACL_BUF_FULL;

	_cmd_bufr[_bwx++ & 0x7f] = data;

	return YACL_SUCCESS;
}

yacl_error_t yacl_parse_cmd()
{
	static uint8_t argc = 0;
	static char* argv[YACL_MAX_ARGS];

	static uint8_t token_begin = 0;
	int32_t argv_cb;

	while(1)
	{
		switch (_cmd_bufr[_brx & 0x7f])
		{
		case DELIM_SPACE:
			if (_cmd_bufr[(_brx - 1) & 0x7f] == '\0')
			{
				_cmd_bufr[_brx & 0x7f] = '\0';
				++token_begin;

				break;
			}

			_save_token(argc, argv, &token_begin);
			++argc;

			if (argc == YACL_MAX_ARGS)
				return YACL_ARGS_FULL;

			break;

		case DELIM_NEWLINE:
			if (_cmd_bufr[(_brx - 1) & 0x7f] == '\0')
			{
				_cmd_bufr[_brx & 0x7f] = '\0';
				++token_begin;

				break;
			}

			_save_token(argc, argv, &token_begin);
			++argc;

			argv_cb = _get_argv_cb();
			_argv_token_zero = token_begin;

			if (argv_cb == NO_CB)
				return YACL_UNKNOWN_CMD;
			else
			{
				_usr_cmd[argv_cb].usr_cmd_cb(argc, argv);
				argc = 0;

				return YACL_SUCCESS;
			}
		}

		if (((_brx + 1) & 0x7f) == (_bwx & 0x7f))
			return YACL_NO_CMD;
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
		{ YACL_SUCCESS,         "command successfully executed"             },
		{ YACL_UNKNOWN_CMD,     "command was not found in given commands"   },
		{ YACL_NO_CMD,          "command incomplete"                        },
		{ YACL_BUF_FULL,        "buffer full. try emptying buffer"          },
		{ YACL_ARGS_FULL,       "args full"                                 }
	};

	if (error >= sizeof error_desc / sizeof (error_desc[0]))
		return NULL;
	else
		return error_desc[error].msg;

}

//      PRIVATE     ****************************************************************************************************

static int32_t _get_argv_cb()
{
	for(uint32_t i = 0; i < _usr_cmd_size; ++i)
	{
		char* str_cmd = _usr_cmd[i].usr_cmd;

		while(1)
		{
			if (_cmd_bufr[_argv_token_zero] == *str_cmd)
			{
				if (_cmd_bufr[_argv_token_zero] == '\0')
					return i;

				++_argv_token_zero;
				++str_cmd;
			}
			else
				break;

		}
	}

	return NO_CB;
}

static void _save_token(uint8_t argc, char** argv, uint8_t* token_begin)
{
	_cmd_bufr[_brx & 0x7f] = '\0';
	argv[argc] = _cmd_bufr + *token_begin;

	*token_begin = (_brx + 1) & 0x7f;
}

static yacl_error_t _buf_chk()
{
	if ((_bwx & 0x80) == (_brx & 0x80))
		return YACL_SUCCESS;
	else if ((_bwx | 0x80) == (_brx | 0x80))
		return YACL_BUF_FULL;

	return YACL_SUCCESS;
}
