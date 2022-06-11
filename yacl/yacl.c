#include "yacl.h"
#include "yacl_types.h"

#include "vt100/vt100.h"

//      GLOBALS

usr_printf_t yacl_printf;
usr_snprintf_t yacl_snprintf;

cb_funcs_t gpio_funcs[2];
cb_funcs_t i2c_funcs[2];

static comm_lut_cb_t g_comm_lut_cb = {
	.protocols = { "gpio", "i2c" },
	.actions = { "read", "write" },
	.funcs = { gpio_funcs, i2c_funcs },

	.num_protocols = sizeof (g_comm_lut_cb.protocols) / sizeof (g_comm_lut_cb.protocols[0]),
	.num_actions = sizeof (g_comm_lut_cb.actions) / sizeof (g_comm_lut_cb.actions[0])
};

static ring_buffer_t g_input_bufr = { .bufr = { 0 }, .head = 0, .tail = 0 };

static data_buffer_t g_tok_bufr = { .bufr = { 0 }, .idx= 0, .tok_array= { NULL }, .tok_beg_idx = 0, .tok_idx = 0 };

//      FUNCTION PROTOTYPES

static yacl_error_t proc_in_bufr();
static yacl_error_t get_comm_lut_idxs(uint32_t* protocol_idx, uint32_t* action_idx);
static _Bool        compare_tokens(const char* str_lhs, const char* str_rhs);

static yacl_error_t bufr_chk();
static void         empty_bufrs();
static void         empty_tok_bufr();
static void         psh_token();
//static void         pop_token();
static void         nxt_token();
static void         null_term_token();
static void         wrt_token(uint8_t data);

//      PUBLIC      ****************************************************************************************************

void yacl_init(yacl_usr_callbacks_t* usr_callbacks)
{
	yacl_printf = usr_callbacks->usr_print_funcs.usr_printf;
	yacl_snprintf = usr_callbacks->usr_print_funcs.usr_snprintf;

	g_comm_lut_cb.funcs[GPIO_CB_IDX][READ_CB_IDX] = usr_callbacks->gpio_funcs[READ_CB_IDX];
	g_comm_lut_cb.funcs[GPIO_CB_IDX][WRITE_CB_IDX] = usr_callbacks->gpio_funcs[WRITE_CB_IDX];

	g_comm_lut_cb.funcs[I2C_CB_IDX][READ_CB_IDX] = usr_callbacks->i2c_funcs[READ_CB_IDX];
	g_comm_lut_cb.funcs[I2C_CB_IDX][WRITE_CB_IDX] = usr_callbacks->i2c_funcs[WRITE_CB_IDX];

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

	uint32_t protocol_idx = 0;
	uint32_t action_idx = 0;

	error = get_comm_lut_idxs(&protocol_idx, &action_idx);

	if (error != YACL_SUCCESS)
	{
		empty_bufrs();
		return error;
	}
	else
	{
		uint32_t data[1];
		uint32_t data_size = 1;

		if (g_tok_bufr.tok_array[2][0] == '1')
			data[0] = 1;
		else if (g_tok_bufr.tok_array[2][0] == '0')
			data[0] = 0;

		g_comm_lut_cb.funcs[protocol_idx][action_idx](data, data_size);

		if (action_idx == READ_CB_IDX)
			yacl_printf("gpio love level: %d\n", data[0]);

		empty_bufrs();
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

			psh_token();

			if (g_tok_bufr.tok_idx >= MAX_TOKENS)
			{
				empty_bufrs();
				return YACL_BUFRS_EMPTD;
			}

			nxt_token();
			break;

		case DELIM_NEWLINE:
			// handle repeated control characters
			if (prev_data == DELIM_NEWLINE)
				break;

			psh_token();

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
			wrt_token(data);
		}

		prev_data = data;
	}

	return YACL_NO_CMD;
}

static yacl_error_t get_comm_lut_idxs(uint32_t* protocol_idx, uint32_t* action_idx)
{
	_Bool token_is_valid = 0;

	for ( ; *protocol_idx < g_comm_lut_cb.num_protocols; ++*protocol_idx)
	{
		token_is_valid = compare_tokens(g_comm_lut_cb.protocols[*protocol_idx], (char*)g_tok_bufr.tok_array[0]);

		if (token_is_valid)
			break;
	}

	if (!token_is_valid)
		return YACL_UNKNOWN_CMD;

	for ( ; *action_idx < g_comm_lut_cb.num_actions; ++*action_idx)
	{
		token_is_valid = compare_tokens(g_comm_lut_cb.actions[*action_idx], (char*)g_tok_bufr.tok_array[1]);

		if (token_is_valid)
			break;
	}

	if (!token_is_valid)
		return YACL_UNKNOWN_CMD;

	return YACL_SUCCESS;
}

static _Bool compare_tokens(const char* str_lhs, const char* str_rhs)
{
    uint32_t str_idx = 0;

	while (1)
	{
		if (str_lhs[str_idx] == str_rhs[str_idx])
		{
			if (str_lhs[str_idx] == '\0')
				return 1;
			else
				++str_idx;
		}
		else
			return 0;
	}
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

void psh_token()
{
	g_tok_bufr.tok_array[g_tok_bufr.tok_idx++] = g_tok_bufr.tok_beg_idx + g_tok_bufr.bufr;
	null_term_token();
}

void nxt_token()
{
	g_tok_bufr.tok_beg_idx = g_tok_bufr.idx;
}

void null_term_token()
{
	g_tok_bufr.bufr[g_tok_bufr.idx++ & 0x7f] = '\0';
}

void wrt_token(uint8_t data)
{
	g_tok_bufr.bufr[g_tok_bufr.idx++ & 0x7f] = data;
}
