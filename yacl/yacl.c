#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

#include "yacl.h"
#include "yacl_types.h"

#include "vt100/vt100.h"

//      GLOBALS

usr_printf_t yacl_printf;
usr_snprintf_t yacl_snprintf;

static protocol_lut_cb_t g_cmd_cbs = {
	.protocols = { "gpio", "i2c", "help" },
	.actions = { "read", "write" },
	.funcs = { { NULL, NULL } },
	.not_null_cbs = { false },
	.num_not_null_cbs = 0
};

static ring_buffer_t g_input_bufr = { .bufr = { 0 }, .head = 0, .tail = 0 };
static data_buffer_t g_tok_bufr = { .bufr = { 0 }, .idx= 0, .tok_array= { NULL }, .tok_cnt = 0 };

//      FUNCTION PROTOTYPES

static yacl_error_t proc_in_bufr();
static yacl_error_t get_protocol_lut_idxs(uint32_t* protocol_idx, uint32_t* action_idx);
static bool         compare_tokens(const char* str_lhs, const char* str_rhs);

static yacl_error_t bufr_chk();
static void         empty_bufrs();
static void         empty_tok_bufr();

static void         psh_token();
static void         pop_token();
static void         null_term_token();
static bool         wrt_to_token(uint8_t data);
static void         rm_from_token(bool dec_idx);

static bool         check_arg_count(uint32_t action);
static bool         conv_args_num(yacl_inout_data_t* inout_data, uint32_t action);
static bool         get_str_value(uint32_t* data, uint32_t tok_idx);
static uint32_t     get_prefix(const uint8_t* num_str, uint32_t num_str_size);
static yacl_error_t get_reg_range(yacl_inout_data_t* inout_data);

static void         help_func(yacl_inout_data_t* inout_data);
static void         init_cbs(yacl_usr_callbacks_t* usr_callbacks);

//      PUBLIC      ****************************************************************************************************

void yacl_init(yacl_usr_callbacks_t* usr_callbacks)
{
	yacl_printf = usr_callbacks->usr_print_funcs.usr_printf;
	yacl_snprintf = usr_callbacks->usr_print_funcs.usr_snprintf;

	init_cbs(usr_callbacks);

	for (uint32_t i = 0; i < MAX_TOKENS; ++i)
		g_tok_bufr.tok_array[i] = g_tok_bufr.bufr + (MAX_TOKEN_LEN * i);

	vt100_rst_term();
	yacl_printf("YACL by Nick\n\nExplore peripherals connected to your MCU freely!\nType 'help' for more information or visit my GitHub\n\n>> ");
}

// TODO: make a global state variable to handle input buffer overrun error
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

	error = get_protocol_lut_idxs(&protocol_idx, &action_idx);

	if (error != YACL_SUCCESS)
	{
		empty_bufrs();
		vt100_error(yacl_error_desc(error));

		return error;
	}

	if (protocol_idx == HELP_CB_IDX && g_tok_bufr.tok_cnt == 1)
	{
		yacl_inout_data_t inout_data;
		g_cmd_cbs.funcs[protocol_idx][READ_CB_IDX](&inout_data);

		empty_bufrs();
		vt100_yacl_view();

		return YACL_SUCCESS;
	}

	if (check_arg_count(action_idx) == false)
	{
		empty_bufrs();
		vt100_error(yacl_error_desc(error));

		return YACL_UNKNOWN_CMD;
	}

	yacl_inout_data_t inout_data;

	if(!conv_args_num(&inout_data, action_idx))
		return YACL_UNKNOWN_CMD;

	if (g_cmd_cbs.not_null_cbs[protocol_idx])
	{
		if (get_reg_range(&inout_data) == YACL_SUCCESS)
			g_cmd_cbs.funcs[protocol_idx][action_idx](&inout_data);
		else
		{
			empty_bufrs();
			vt100_error(yacl_error_desc(YACL_INOUT_BUFR));

			return YACL_UNKNOWN_CMD;
		}
	}
	else
	{
		empty_bufrs();
		vt100_error(yacl_error_desc(YACL_NO_CALLBACK));

		return YACL_UNKNOWN_CMD;
	}

	if (action_idx == READ_CB_IDX)
	{
		uint32_t start = inout_data.beg_reg;

		if (inout_data.beg_reg > inout_data.end_reg)
			start = inout_data.end_reg;

		yacl_printf("\n");
		for (uint32_t i = 0; i < inout_data.range; ++i)
			yacl_printf("0x%04x  ::  %d\n", start + i, inout_data.bufr[i]);

		yacl_printf("\n");
	}

	empty_bufrs();
	vt100_yacl_view();

	return YACL_SUCCESS;
}

void yacl_set_cb_null(yacl_usr_callbacks_t* usr_callbacks)
{
	usr_callbacks->usr_gpio_read = NULL;
	usr_callbacks->usr_gpio_write = NULL;

	usr_callbacks->usr_i2c_read = NULL;
	usr_callbacks->usr_i2c_write = NULL;
}

const char* yacl_error_desc(yacl_error_t error)
{
	static const error_desc_t error_desc[] = {
		{ YACL_SUCCESS,         "no errors. good to go"                     },
		{ YACL_UNKNOWN_CMD,     "command was not found in given commands"   },
		{ YACL_NO_CMD,          "command incomplete"                        },
		{ YACL_BUF_FULL,        "buffer full. try emptying buffer"          },
		{ YACL_BUFRS_EMPTD,     "buffers emptied"                           },
		{ YACL_ARG_OVRN,        "arg was too long. max 14 characters"       },
		{ YACL_TOO_MANY_ARGS,   "command has too many args"                 },
		{ YACL_NOT_ENUF_ARGS,   "command missing args"                      },
		{ YACL_INVALID_ARG,     "invalid arg"                               },
		{ YACL_INOUT_BUFR,      "register range greater than inout buffer"  },
		{ YACL_NO_CALLBACK,     "command was not given a callback function" },
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
				vt100_error(yacl_error_desc(YACL_TOO_MANY_ARGS));

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
				vt100_error(yacl_error_desc(YACL_ARG_OVRN));

				return YACL_BUFRS_EMPTD;
			}
		}

		prev_data = data;
	}

	return YACL_NO_CMD;
}

static yacl_error_t get_protocol_lut_idxs(uint32_t* protocol_idx, uint32_t* action_idx)
{
	bool token_is_valid = false;

	for ( ; *protocol_idx < NUM_PROTOCOLS; ++*protocol_idx)
	{
		token_is_valid = compare_tokens(g_cmd_cbs.protocols[*protocol_idx], (char*)g_tok_bufr.tok_array[ARG_PROTOCOL_IDX]);

		if (token_is_valid)
			break;
	}

	if (!token_is_valid && *protocol_idx != HELP_CB_IDX)
		return YACL_UNKNOWN_CMD;
	else if (*protocol_idx == HELP_CB_IDX)
		return YACL_SUCCESS;

	for ( ; *action_idx < NUM_ACTIONS; ++*action_idx)
	{
		token_is_valid = compare_tokens(g_cmd_cbs.actions[*action_idx], (char*)g_tok_bufr.tok_array[ARG_ACTION_IDX]);

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
//	g_tok_bufr.tok_array[g_tok_bufr.tok_cnt][TOKENS_LEN_IDX] = g_tok_bufr.idx;
	g_tok_bufr.tok_array[g_tok_bufr.tok_cnt][TOKENS_LEN_IDX] = g_tok_bufr.idx + 1;
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

static bool check_arg_count(uint32_t action)
{
	if (action == READ_CB_IDX)
	{
		if (g_tok_bufr.tok_cnt >= TOKEN_CNT_READ_MIN)
			return true;
		else
			return false;
	}
	else // if (action == WRITE_CB_IDX)
	{
		if (g_tok_bufr.tok_cnt >= TOKEN_CNT_WRITE_MIN)
			return true;
		else
			return false;
	}
}

static bool conv_args_num(yacl_inout_data_t* inout_data, uint32_t action)
{
	if (get_str_value(&inout_data->addr, ARG_ADDR_IDX) == false)
		return false;

	if (get_str_value(&inout_data->beg_reg, ARG_BEG_REG_IDX) == false)
		return false;

	if (action == READ_CB_IDX)
	{
		if (g_tok_bufr.tok_cnt > ARG_READ_RANGE_CNT)
		{
			if (get_str_value(&inout_data->end_reg, ARG_END_REG_IDX) == false)
				return false;
			return true;
		}

		inout_data->end_reg = inout_data->beg_reg;
		return true;
	}
	else // if (action == WRITE_CB_IDX)
	{
		if (g_tok_bufr.tok_cnt > ARG_WRITE_RANGE_CNT)
		{
			if (get_str_value(&inout_data->end_reg, ARG_END_REG_IDX) == false)
				return false;
			if (get_str_value(&inout_data->data, ARG_DATA1_IDX) == false)
				return false;
			return true;
		}

		inout_data->end_reg = inout_data->beg_reg;
		if (get_str_value(&inout_data->data, ARG_DATA0_IDX) == false)
			return false;
		return true;
	}
}

static bool get_str_value(uint32_t* data, uint32_t tok_idx)
{
	uint32_t base = get_prefix(g_tok_bufr.tok_array[tok_idx], g_tok_bufr.tok_array[tok_idx][TOKENS_LEN_IDX]);
	char * end_ptr = (char*)(g_tok_bufr.tok_array[tok_idx] + g_tok_bufr.tok_array[tok_idx][TOKENS_LEN_IDX]);

	uint32_t i = 0;
	if (base == 16)
		i = 2;

	for (; i < g_tok_bufr.tok_array[tok_idx][TOKENS_LEN_IDX] - 1; ++i)
	{
		if (!isxdigit(g_tok_bufr.tok_array[tok_idx][i]))
		{
			empty_bufrs();
			vt100_error_data(yacl_error_desc(YACL_INVALID_ARG), tok_idx + 1);

			return false;
		}
	}
	uint32_t check_result = strtoul((char*)g_tok_bufr.tok_array[tok_idx], &end_ptr, base);

	if (check_result == ULONG_MAX)
	{
		empty_bufrs();
		vt100_error_data(yacl_error_desc(YACL_INVALID_ARG), tok_idx + 1);

		return false;
	}

	*data = check_result;

	return true;
}

yacl_error_t get_reg_range(yacl_inout_data_t* inout_data)
{
	if (inout_data->beg_reg > inout_data->end_reg)
		inout_data->range = (inout_data->beg_reg - inout_data->end_reg) + 1;
	else if (inout_data->beg_reg < inout_data->end_reg)
		inout_data->range = (inout_data->end_reg - inout_data->beg_reg) + 1;
	else
		inout_data->range = 1;

	if (inout_data->range > INOUT_BUFR_LEN)
		return YACL_INOUT_BUFR;

	return YACL_SUCCESS;
}

static uint32_t get_prefix(const uint8_t* num_str, uint32_t num_str_size)
{
	if (num_str[0] == '0' && (num_str[1] == 'x' || num_str[1] == 'X') )
		return 16;
	else
		return 10;
}

void init_cbs(yacl_usr_callbacks_t* usr_callbacks)
{
	g_cmd_cbs.funcs[HELP_CB_IDX][0] = help_func;
//	g_cmd_cbs.funcs[HELP_CB_IDX][1] = help_func;

	if (usr_callbacks->usr_gpio_read && usr_callbacks->usr_gpio_write)
	{
		g_cmd_cbs.funcs[GPIO_CB_IDX][READ_CB_IDX] = usr_callbacks->usr_gpio_read;
		g_cmd_cbs.funcs[GPIO_CB_IDX][WRITE_CB_IDX] = usr_callbacks->usr_gpio_write;

		g_cmd_cbs.not_null_cbs[GPIO_CB_IDX] = true;
		++g_cmd_cbs.num_not_null_cbs;
	}

	if (usr_callbacks->usr_i2c_read && usr_callbacks->usr_i2c_write)
	{
		g_cmd_cbs.funcs[I2C_CB_IDX][READ_CB_IDX] = usr_callbacks->usr_i2c_read;
		g_cmd_cbs.funcs[I2C_CB_IDX][WRITE_CB_IDX] = usr_callbacks->usr_i2c_write;

		g_cmd_cbs.not_null_cbs[I2C_CB_IDX] = true;
		++g_cmd_cbs.num_not_null_cbs;
	}
}

static void help_func(yacl_inout_data_t* inout_data)
{
	yacl_printf("\nYACL Help\n\n\t<protocol> <action> <addr> <reg> [reg end] [data]\n\n");

	for (uint32_t i = 0; i < g_cmd_cbs.num_not_null_cbs; ++i)
		yacl_printf("%s supports read and write\n", g_cmd_cbs.protocols[i]);
	yacl_printf("\n");
}
