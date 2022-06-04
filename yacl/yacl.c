#include <stddef.h>
#include <stdio.h>

#include "yacl.h"

//      TYPES

enum
{
	NO_CB               = -1,

	DELIM_SPACE         = 32,
	DELIM_NEWLINE       = 10,

	YACL_CMD_LEN_MAX    = 128,
	YACL_MAX_ARGS       = 12
};

typedef struct error_desc {
	const yacl_error_t code;
	const char* msg;

} error_desc_t;

//      GLOBALS

static yacl_cmd_cb_t* g_usr_cmd;
static uint32_t g_usr_cmd_size;

static void (*g_print_func)(char data);

static char g_cmd_bufr[YACL_CMD_LEN_MAX];
static uint8_t g_brx = 0;
static uint8_t g_bwx = 0;

static uint8_t g_arr_tok_bufr_idx = 0;
static uint8_t g_tok_beg_idx = 0;

static int8_t g_tok_cur_idx = 0;

//      FUNCTION PROTOTYPES

static yacl_error_t bufr_chk();
static yacl_error_t proc_in_bufr(uint8_t* tok_bufr, uint8_t** array_tok_bufr);
static int32_t      get_argv_cb(const uint8_t* tok_bufr);
static void         empty_bufr();

//      PUBLIC      ****************************************************************************************************

void yacl_init(yacl_cmd_cb_t* usr_cmd, uint32_t usr_cmd_size, void (* print_func)(char))
{
	g_usr_cmd = usr_cmd;
	g_usr_cmd_size = usr_cmd_size;

	g_print_func = print_func;
}

yacl_error_t yacl_wr_buf(char data)
{
	if (bufr_chk() != YACL_SUCCESS)
		return YACL_BUF_FULL;

	g_cmd_bufr[g_bwx++ & 0x7f] = data;

	return YACL_SUCCESS;
}

yacl_error_t yacl_parse_cmd()
{
	static uint8_t tok_bufr[YACL_CMD_LEN_MAX] = { 0 };
	static uint8_t* array_tok_bufr[YACL_MAX_ARGS] = { NULL };

	yacl_error_t error;
	error = proc_in_bufr(tok_bufr, array_tok_bufr);

	if (error != YACL_SUCCESS)
		return error;

	int32_t cb_idx = get_argv_cb(tok_bufr);

	if (cb_idx == NO_CB)
		return YACL_UNKNOWN_CMD;
	else
		g_usr_cmd[cb_idx].usr_cmd_cb(g_arr_tok_bufr_idx, (char**)array_tok_bufr);

	g_arr_tok_bufr_idx = 0;
	g_tok_beg_idx = 0;
	g_tok_cur_idx = 0;

	return YACL_SUCCESS;
}

const char* yacl_error_desc(yacl_error_t error)
{
	static const error_desc_t error_desc[] = {
		{ YACL_SUCCESS,         "no errors. good to go"                     },
		{ YACL_UNKNOWN_CMD,     "command was not found in given commands"   },
		{ YACL_NO_CMD,          "command incomplete"                        },
		{ YACL_BUF_FULL,        "buffer full. try emptying buffer"          },
		{ YACL_ARGS_FULL,       "args full"                                 },
		{ YACL_INT_OVRN,        "internal buffer overrun"                   },
		{ YACL_BUFR_EMPTD,      "buffers emptied"                           }
	};

	return error_desc[error].msg;
}

//      PRIVATE     ****************************************************************************************************

void empty_bufr()
{
	g_brx = 0;
	g_bwx = 0;

	g_arr_tok_bufr_idx = 0;
	g_tok_beg_idx = 0;
	g_tok_cur_idx = 0;
}

static int32_t get_argv_cb(const uint8_t* tok_bufr)
{
	uint8_t tok_bufr_idx = 0;

	for(uint32_t i = 0; i < g_usr_cmd_size; ++i)
	{
		char* str_cmd = g_usr_cmd[i].usr_cmd;

		while(1)
		{
			if (tok_bufr[tok_bufr_idx] == *str_cmd)
			{
				if (tok_bufr[tok_bufr_idx] == '\0')
					return i;

				++tok_bufr_idx;
				++str_cmd;
			}
			else
				break;
		}
	}

	return NO_CB;
}

static yacl_error_t bufr_chk()
{
	if ((g_bwx & 0x80) == (g_brx & 0x80))
		return YACL_SUCCESS;
	else if ((g_bwx | 0x80) == ((g_brx + 1) | 0x80))
		return YACL_BUF_FULL;

	return YACL_SUCCESS;
}

static yacl_error_t proc_in_bufr(uint8_t* tok_bufr, uint8_t** array_tok_bufr)
{
	int8_t num_new_bytes = (g_bwx - g_brx) % YACL_CMD_LEN_MAX;

	if (num_new_bytes <= 1)
		return YACL_NO_CMD;

	while(num_new_bytes--)
	{
		uint8_t data = g_cmd_bufr[g_brx++ & 0x7f];

		switch (data)
		{
		case DELIM_SPACE:
			g_cmd_bufr[(g_brx - 1) & 0x7f] = '\0';

			if (g_cmd_bufr[(g_brx - 2) & 0x7f] == '\0')
			{
				++g_tok_beg_idx;
				break;
			}

			tok_bufr[g_tok_cur_idx++] = '\0';
			array_tok_bufr[g_arr_tok_bufr_idx++] = tok_bufr + g_tok_beg_idx;

			g_tok_beg_idx = g_tok_cur_idx;

			if (g_arr_tok_bufr_idx >= YACL_MAX_ARGS)
			{
				empty_bufr();
				return YACL_ARGS_FULL;
			}

			break;

		case DELIM_NEWLINE:
			array_tok_bufr[g_arr_tok_bufr_idx++] = tok_bufr + g_tok_beg_idx;
			tok_bufr[g_tok_cur_idx] = '\0';

			return YACL_SUCCESS;

		default:
			tok_bufr[g_tok_cur_idx++] = data;
		}

		if (g_tok_cur_idx < 0)
		{
			empty_bufr();
			return YACL_INT_OVRN;
		}
	}

	return YACL_NO_CMD;
}
