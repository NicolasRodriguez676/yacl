#include "yacl.h"
#include "vt100/vt100.h"

//      TYPES

enum Misc {
	NO_CB   = -1,
};

enum Control_Characters {
	DELIM_SPACE         = 32,
	DELIM_NEWLINE       = 10,
	CNTRL_BACKSPACE     = 8,
};

enum Buffer_Lengths {
	DATA_LEN_MAX     = 128,
	MAX_TOKENS       = 12
};

typedef struct error_desc {
	const yacl_error_t code;
	const char* msg;

} error_desc_t;

typedef struct ring_buffer {
	uint8_t bufr[DATA_LEN_MAX];
	uint32_t head;
	uint32_t tail;

} ring_buffer_t;

typedef struct data_buffer {
	uint8_t bufr[DATA_LEN_MAX];
	uint32_t idx;

	uint8_t* tok_array[MAX_TOKENS];
	uint32_t tok_beg_idx;
	uint32_t tok_idx;

} data_buffer_t;

//      GLOBALS

extern void (*print_func)(char data);

static yacl_cmd_cb_t* g_usr_cmd;
static uint32_t g_usr_cmd_size;

static ring_buffer_t g_input_bufr = {
	.bufr = { 0 },
	.head = 0,
	.tail = 0
};

static data_buffer_t g_tok_bufr = {
	.bufr           = { 0 },
	.idx            = 0,
	.tok_array      = { NULL },
	.tok_beg_idx    = 0,
	.tok_idx        = 0
};

const char* lut = "0123456789abcdef";

//      FUNCTION PROTOTYPES

static yacl_error_t proc_in_bufr();
static int32_t      get_argv_cb();

static yacl_error_t bufr_chk();
static void         empty_bufrs();
static void         empty_tok_bufr();
//static void         psh_token();
//static void         pop_token();
//static void         wrt_token(uint8_t data);

//      PUBLIC      ****************************************************************************************************

void yacl_init(yacl_cmd_cb_t* usr_cmd, uint32_t usr_cmd_size, void (* usr_print_func)(char))
{
	g_usr_cmd = usr_cmd;
	g_usr_cmd_size = usr_cmd_size;

	print_func = usr_print_func;

	vt100_rst_term();
}

yacl_error_t yacl_wr_buf(char data)
{
	if (bufr_chk() == YACL_BUF_FULL)
	{
		empty_bufrs();
		return YACL_BUFRS_EMPTD;
	}

	g_input_bufr.bufr[g_input_bufr.head++ & 0x7f] = data;

	return YACL_SUCCESS;
}

yacl_error_t yacl_parse_cmd()
{
	yacl_error_t error = proc_in_bufr();

	if (error != YACL_SUCCESS)
		return error;

	int32_t cb_idx = get_argv_cb();

	if (cb_idx == NO_CB)
	{
		empty_tok_bufr();
		return YACL_UNKNOWN_CMD;
	}
	else
	{
		if (cb_idx == 1)
		{
			uint32_t data_in;
			g_usr_cmd[cb_idx].usr_cmd_cb(g_tok_bufr.tok_idx, (char**)g_tok_bufr.tok_array, &data_in);
			print_func('\n');
			print_func('[');
			print_func(lut[data_in & 0x0f]);
			print_func(']');
			print_func('\n');
		}
		else
			g_usr_cmd[cb_idx].usr_cmd_cb(g_tok_bufr.tok_idx, (char**)g_tok_bufr.tok_array, NULL);

		empty_tok_bufr();

		return YACL_SUCCESS;
	}
}

const char* yacl_error_desc(yacl_error_t error)
{
	static const error_desc_t error_desc[] = {
		{ YACL_SUCCESS,         "no errors. good to go"                     },
		{ YACL_UNKNOWN_CMD,     "command was not found in given commands"   },
		{ YACL_NO_CMD,          "command incomplete"                        },
		{ YACL_BUF_FULL,        "buffer full. try emptying buffer"          },
		{ YACL_BUFRS_EMPTD,     "buffers emptied"                           }
	};

	return error_desc[error].msg;
}

//      PRIVATE     ****************************************************************************************************

static yacl_error_t proc_in_bufr()
{
	static uint8_t prev_data = DELIM_SPACE;

	int8_t num_new_bytes = g_input_bufr.head  - g_input_bufr.tail;

	if (num_new_bytes < 1)
		return YACL_NO_CMD;

	while(num_new_bytes--)
	{
		uint8_t data = g_input_bufr.bufr[g_input_bufr.tail++ & 0x7f];

		switch (data)
		{
		case DELIM_SPACE:
			// handle repeated control characters
			if (prev_data == DELIM_SPACE)
				break;

			// save token. check if enough space left for last arg
			g_tok_bufr.tok_array[g_tok_bufr.tok_idx++] = g_tok_bufr.tok_beg_idx + g_tok_bufr.bufr;
			if (g_tok_bufr.tok_idx >= MAX_TOKENS)
			{
				empty_bufrs();
				return YACL_BUFRS_EMPTD;
			}

			// terminate token in data buffer. start new token offset
			g_tok_bufr.bufr[g_tok_bufr.idx++ & 0x7f] = '\0';
			g_tok_bufr.tok_beg_idx = g_tok_bufr.idx;

			break;

		case DELIM_NEWLINE:
			// handle repeated control characters
			if (prev_data == DELIM_NEWLINE)
				break;

			// save and terminate token. return success to proceed into further processing
			g_tok_bufr.bufr[g_tok_bufr.idx & 0x7f] = '\0';
			g_tok_bufr.tok_array[g_tok_bufr.tok_idx++] = g_tok_bufr.tok_beg_idx + g_tok_bufr.bufr;

			prev_data = DELIM_SPACE;
			return YACL_SUCCESS;

		case CNTRL_BACKSPACE:
			// TODO: recover token space

			if (g_tok_bufr.idx == 0 && prev_data == CNTRL_BACKSPACE)
				break;
			else if (g_tok_bufr.idx == 0)
				g_tok_bufr.bufr[g_tok_bufr.idx & 0x7f] = '\0';
			else
				g_tok_bufr.bufr[--g_tok_bufr.idx & 0x7f] = '\0';

			vt100_backspace();
			break;

		default:
			g_tok_bufr.bufr[g_tok_bufr.idx++ & 0x7f] = data;
		}

		prev_data = data;
	}

	return YACL_NO_CMD;
}

static int32_t get_argv_cb()
{
	uint8_t tok_bufr_idx = 0;

	for(uint32_t i = 0; i < g_usr_cmd_size; ++i)
	{
		char* str_cmd = g_usr_cmd[i].usr_cmd;

		while(1)
		{
			if (g_tok_bufr.bufr[tok_bufr_idx] == *str_cmd)
			{
				if (g_tok_bufr.bufr[tok_bufr_idx] == '\0')
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
	uint8_t empty_bytes = DATA_LEN_MAX - (g_input_bufr.head - g_input_bufr.tail);

	if (empty_bytes <= 1)
		return YACL_BUF_FULL;
	else
		return YACL_SUCCESS;
}

static void empty_bufrs()
{
	g_input_bufr.head = 0;
	g_input_bufr.tail = 0;

	empty_tok_bufr();
}

void empty_tok_bufr()
{
	g_tok_bufr.tok_beg_idx = 0;
	g_tok_bufr.tok_idx = 0;
	g_tok_bufr.idx = 0;
}
