#include <stdbool.h>

#include "yacl.h"
#include "yacl_types.h"

#include "vt100/vt100.h"

//      GLOBALS

usr_printf_t yacl_printf;
usr_snprintf_t yacl_snprintf;

static cb_funcs_t gpio_funcs[2];
static cb_funcs_t i2c_funcs[2];

static comm_lut_cb_t g_cmd_cbs = {
	.protocols = { "gpio", "i2c" },
	.actions = { "read", "write" },
	.funcs = { gpio_funcs, i2c_funcs },

	.num_protocols = sizeof (g_cmd_cbs.protocols) / sizeof (g_cmd_cbs.protocols[0]),
	.num_actions = sizeof (g_cmd_cbs.actions) / sizeof (g_cmd_cbs.actions[0])
};

static ring_buffer_t g_input_bufr = { .bufr = { 0 }, .head = 0, .tail = 0 };

static data_buffer_t g_tok_bufr = { .bufr = { 0 }, .idx= 0, .tok_array= { NULL }, .tok_cnt = 0 };

//      FUNCTION PROTOTYPES

static yacl_error_t proc_in_bufr();
static yacl_error_t get_comm_lut_idxs(uint32_t* protocol_idx, uint32_t* action_idx);
static bool         compare_tokens(const char* str_lhs, const char* str_rhs);

static yacl_error_t bufr_chk();
static void         empty_bufrs();
static void         empty_tok_bufr();
static void         psh_token();
static void         pop_token();
static void         null_term_token();
static bool         wrt_to_token(uint8_t data);
static void         rm_from_token(bool dec_idx);

//      PUBLIC      ****************************************************************************************************

void yacl_init(yacl_usr_callbacks_t* usr_callbacks)
{
	yacl_printf = usr_callbacks->usr_print_funcs.usr_printf;
	yacl_snprintf = usr_callbacks->usr_print_funcs.usr_snprintf;

	g_cmd_cbs.funcs[GPIO_CB_IDX][READ_CB_IDX] = usr_callbacks->gpio_funcs[READ_CB_IDX];
	g_cmd_cbs.funcs[GPIO_CB_IDX][WRITE_CB_IDX] = usr_callbacks->gpio_funcs[WRITE_CB_IDX];

	g_cmd_cbs.funcs[I2C_CB_IDX][READ_CB_IDX] = usr_callbacks->i2c_funcs[READ_CB_IDX];
	g_cmd_cbs.funcs[I2C_CB_IDX][WRITE_CB_IDX] = usr_callbacks->i2c_funcs[WRITE_CB_IDX];

	for (uint32_t i = 0; i < MAX_TOKENS; ++i)
		g_tok_bufr.tok_array[i] = g_tok_bufr.bufr + (MAX_TOKEN_LEN * i);

	vt100_rst_term();
	vt100_yacl_view();
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

		g_cmd_cbs.funcs[protocol_idx][action_idx](data, data_size);

		if (action_idx == READ_CB_IDX)
			yacl_printf("gpio love level: %d\n\n", data[0]);

		empty_bufrs();
		vt100_yacl_view();

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

	uint32_t num_new_bytes = g_input_bufr.head - g_input_bufr.tail;
	bool dec_idx_when_rmv;

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

			if (g_tok_bufr.tok_cnt >= MAX_TOKENS)
			{
				empty_bufrs();

				vt100_error("ERROR: Too many tokens");
				vt100_yacl_view();

				return YACL_BUFRS_EMPTD;
			}

			break;

		case DELIM_NEWLINE:
			// handle repeated control characters
			if (prev_data == DELIM_NEWLINE)
				break;

			psh_token();

			prev_data = DELIM_SPACE;
			return YACL_SUCCESS;

		case CNTRL_BACKSPACE:
			if (g_tok_bufr.idx == 0 && g_tok_bufr.tok_cnt == 0)
			{
				dec_idx_when_rmv = false;
				rm_from_token(dec_idx_when_rmv);
			}
			else if (g_tok_bufr.idx == 0)
			{
				pop_token();
				dec_idx_when_rmv = true;
				rm_from_token(dec_idx_when_rmv);
			}
			else
			{
				dec_idx_when_rmv = true;
				rm_from_token(dec_idx_when_rmv);
			}

			vt100_backspace();
			break;

		default:
			if (wrt_to_token(data))
				break;
			else
			{
				empty_bufrs();

				vt100_error("ERROR: Token overrun");
				vt100_yacl_view();

				return YACL_BUFRS_EMPTD;
			}
		}

		prev_data = data;
	}

	return YACL_NO_CMD;
}

static yacl_error_t get_comm_lut_idxs(uint32_t* protocol_idx, uint32_t* action_idx)
{
	bool token_is_valid = false;

	for ( ; *protocol_idx < g_cmd_cbs.num_protocols; ++*protocol_idx)
	{
		token_is_valid = compare_tokens(g_cmd_cbs.protocols[*protocol_idx], (char*)g_tok_bufr.tok_array[0]);

		if (token_is_valid)
			break;
	}

	if (!token_is_valid)
		return YACL_UNKNOWN_CMD;

	for ( ; *action_idx < g_cmd_cbs.num_actions; ++*action_idx)
	{
		token_is_valid = compare_tokens(g_cmd_cbs.actions[*action_idx], (char*)g_tok_bufr.tok_array[1]);

		if (token_is_valid)
			break;
	}

	if (!token_is_valid)
		return YACL_UNKNOWN_CMD;

	return YACL_SUCCESS;
}

static bool compare_tokens(const char* str_lhs, const char* str_rhs)
{
    uint32_t str_idx = 0;

	while (1)
	{
		if (str_lhs[str_idx] == str_rhs[str_idx])
		{
			if (str_lhs[str_idx] == '\0')
				return true;
			else
				++str_idx;
		}
		else
			return false;
	}
}

static yacl_error_t bufr_chk()
{
	uint8_t empty_bytes = MAX_DATA_LEN - (g_input_bufr.head - g_input_bufr.tail);

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

static void empty_tok_bufr()
{
	g_tok_bufr.tok_cnt = 0;
	g_tok_bufr.idx = 0;
}

static void psh_token()
{
	null_term_token();
	g_tok_bufr.idx = 0;
	++g_tok_bufr.tok_cnt;
}

static void pop_token()
{
	g_tok_bufr.idx = g_tok_bufr.tok_array[--g_tok_bufr.tok_cnt][TOKENS_LEN_IDX];
}

static void null_term_token()
{
	g_tok_bufr.tok_array[g_tok_bufr.tok_cnt][TOKENS_LEN_IDX] = g_tok_bufr.idx;
	g_tok_bufr.tok_array[g_tok_bufr.tok_cnt][g_tok_bufr.idx] = '\0';
}

static bool wrt_to_token(uint8_t data)
{
	if (g_tok_bufr.idx + 1 >= MAX_TOKEN_LEN - 2)
		return false;

	g_tok_bufr.tok_array[g_tok_bufr.tok_cnt][g_tok_bufr.idx++] = data;
	return true;
}

static void rm_from_token(bool dec_idx)
{
	if (dec_idx)
		g_tok_bufr.tok_array[g_tok_bufr.tok_cnt][--g_tok_bufr.idx] = '\0';
	else
		g_tok_bufr.tok_array[g_tok_bufr.tok_cnt][g_tok_bufr.idx] = '\0';
}
