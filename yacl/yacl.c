#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "yacl.h"
#include "yacl_types.h"

#include "parser/parser.h"
#include "vt100/vt100.h"

//      GLOBALS

usr_printf_f yacl_printf;
usr_snprintf_f yacl_snprintf;

static cb_lut_t g_cb_lut;
static yacl_inout_data_s g_prev_inout_data;

static ring_buffer_t g_input_bufr = { .bufr = { 0 }, .head = 0, .tail = 0 };

static bool input_bufr_ok = true;
static bool is_plot       = false;

static yacl_graph_s g_graph = {
        .upper_range = 1.0f,
        .lower_range = 0.0f,
        .num_steps   = 11,
        .num_samples = 60,
        .units       = "V",
};

//      FUNCTION PROTOTYPES

static yacl_error_e proc_in_bufr();

static yacl_error_e bufr_chk();
static void         empty_bufr();

static void         init_cbs(cb_lut_t* cb_lut, yacl_usr_callbacks_s* usr_callbacks);
static void         set_callbacks(cb_lut_t* cb_lut, void (**usr_cb)(yacl_inout_data_s *), stream_e stream);

static void         init_graph(yacl_graph_s* usr_graph);

static void         init_walk_stack(walk_stack_s* walk_stack, option_data_stack_s* option_stack, char** args, uint32_t* base);
static void         init_option_stack(option_data_stack_s* opt_stack, char** args, uint32_t* base, uint32_t size, uint32_t start);
static bool         get_stack_data(walk_stack_s* stack, yacl_inout_data_s* inout_data);

static void         set_func(yacl_inout_data_s* prev_inout_data, yacl_inout_data_s* inout_data);
static void         help_func(cb_lut_t* cb_lut);
static void         clear_func();

static yacl_error_e call_action_func(cb_lut_t* cb_lut, action_e action, stream_e stream, yacl_inout_data_s* inout_data, yacl_inout_data_s* prev_inout_data, bool use_prev_io_data);

//      PUBLIC      ****************************************************************************************************

void yacl_init(yacl_usr_callbacks_s* usr_callbacks, yacl_graph_s* usr_graph)
{
    // assume user provides valid printf functions
    yacl_printf = usr_callbacks->usr_print_funcs.usr_printf;
    yacl_snprintf = usr_callbacks->usr_print_funcs.usr_snprintf;

    // initialize callbacks into protocol lookup table
    // record non-null callbacks as valid
    init_cbs(&g_cb_lut, usr_callbacks);

    // set up the graph properties for plotting
    init_graph(usr_graph);

    // display welcome message
    vt100_welcome();
}

void yacl_wr_buf(char data)
{
    // check if input buffer has enough space
    // if not then flag buffer to be erased later
    if (bufr_chk() == YACL_BUF_FULL)
    {
        input_bufr_ok = false;
        return;
    }

    if (is_plot)
    {
        if (data == EXIT_PLOT)
            is_plot = false;
        return;
    }

    g_input_bufr.bufr[g_input_bufr.head++ & 0x7f] = data;
}

yacl_error_e yacl_parse_cmd()
{
    if (g_input_bufr.bufr[(g_input_bufr.head - 1) & 0x7f] != '\n')
        return YACL_NO_CMD;

    char* stack_args[OPT_DATA_SIZE + OPT_REG_SIZE + OPT_ADDR_SIZE + OPT_STATE_SIZE];
    uint32_t stack_base[OPT_DATA_SIZE + OPT_REG_SIZE + OPT_ADDR_SIZE + OPT_STATE_SIZE];
    option_data_stack_s option_stack[NUM_OPTIONS];

    walk_stack_s stack;
    init_walk_stack(&stack, option_stack, stack_args, stack_base);

    yacl_error_e error;
    error = parser((char *) g_input_bufr.bufr, (char *) g_input_bufr.bufr + g_input_bufr.head, &stack);

    if (error != YACL_SUCCESS)
    {
        vt100_yacl_view();
        empty_bufr();
        return error;
    }

    yacl_inout_data_s inout_data;
	bool data_present = get_stack_data(&stack, &inout_data);

    error = call_action_func(&g_cb_lut, stack.action, stack.stream, &inout_data, &g_prev_inout_data, data_present);

	if (error != YACL_SUCCESS)
		vt100_error(yacl_error_desc(YACL_NO_CALLBACK));

    empty_bufr();
    vt100_yacl_view();
    return error;
}

yacl_error_e yacl_plot(float data)
{
    if (is_plot)
    {
        vt100_plot_graph(&g_graph, data);
        return YACL_SUCCESS;
    }
    else
        return YACL_NO_CMD;
}

void yacl_set_cb_null(yacl_usr_callbacks_s* usr_callbacks)
{
    // help users choose only what they want in their build of this program
    // reduces upkeep in their program and visual clutter

    usr_callbacks->usr_gpio_read  = NULL;
    usr_callbacks->usr_gpio_write = NULL;
    usr_callbacks->usr_gpio_plot  = NULL;

    usr_callbacks->usr_i2c_read  = NULL;
    usr_callbacks->usr_i2c_write = NULL;
    usr_callbacks->usr_i2c_plot  = NULL;

    usr_callbacks->usr_spi_read  = NULL;
    usr_callbacks->usr_spi_write = NULL;
    usr_callbacks->usr_spi_plot  = NULL;
}

const char* yacl_error_desc(yacl_error_e error)
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
            { YACL_NO_CALLBACK,    "command was not given a callback function" },
    };

    return error_desc[error].msg;
}

//      PRIVATE     ****************************************************************************************************

static yacl_error_e proc_in_bufr()
{
    return YACL_SUCCESS;

    /*
    // ignore leading and consecutive backspaces
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
                    empty_bufr();
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
                    empty_bufr();
                    vt100_error(yacl_error_desc(YACL_ARG_OVRN));

                    return YACL_BUFRS_EMPTD;
                }
        }

        prev_data = data;
    }

    return YACL_NO_CMD;
    */
}

static yacl_error_e bufr_chk()
{
    uint8_t empty_bytes = MAX_DATA_LEN - (g_input_bufr.head - g_input_bufr.tail);

    if (empty_bytes <= 1)
        return YACL_BUF_FULL;
    else
        return YACL_SUCCESS;
}

static void empty_bufr()
{
    input_bufr_ok = true;

    g_input_bufr.head = 0;
    g_input_bufr.tail = 0;
}

static void init_cbs(cb_lut_t* cb_lut, yacl_usr_callbacks_s *usr_callbacks)
{
	cb_funcs_f usr_cb[NUM_STREAMS];

	usr_cb[STREAM_GPIO] = usr_callbacks->usr_gpio_write;
	usr_cb[STREAM_I2C] = usr_callbacks->usr_gpio_read;
	usr_cb[STREAM_SPI] = usr_callbacks->usr_gpio_plot;
	set_callbacks(cb_lut, usr_cb, STREAM_GPIO);

	usr_cb[STREAM_GPIO] = usr_callbacks->usr_i2c_write;
	usr_cb[STREAM_I2C] = usr_callbacks->usr_i2c_read;
	usr_cb[STREAM_SPI] = usr_callbacks->usr_i2c_plot;
	set_callbacks(cb_lut, usr_cb, STREAM_I2C);

	usr_cb[STREAM_GPIO] = usr_callbacks->usr_spi_write;
	usr_cb[STREAM_I2C] = usr_callbacks->usr_spi_read;
	usr_cb[STREAM_SPI] = usr_callbacks->usr_spi_plot;
	set_callbacks(cb_lut, usr_cb, STREAM_SPI);
}

static void set_callbacks(cb_lut_t* cb_lut, void (**usr_cb)(yacl_inout_data_s *), stream_e stream)
{
	for (action_e action = ACTION_WRITE; action < NUM_USR_DEF_ACTIONS; ++action)
	{
		if (usr_cb[action] != NULL)
		{
			cb_lut->funcs[action][stream] = usr_cb[action];
			cb_lut->not_null_cbs[action][stream] = true;
		}
	}
}

static void init_walk_stack(walk_stack_s *walk_stack, option_data_stack_s *option_stack, char **args, uint32_t *base)
{
	walk_stack->action = ACTION_NONE;
	walk_stack->stream = STREAM_NONE;

	for (uint32_t i = OPT_DATA; i < NUM_OPTIONS; ++i)
	{
		walk_stack->valid_options[i] = false;
		walk_stack->options[i] = option_stack[i];
	}

	init_option_stack(&walk_stack->options[OPT_DATA],  args, base, OPT_DATA_SIZE,  OPT_DATA_OFFSET );
	init_option_stack(&walk_stack->options[OPT_REG],   args, base, OPT_REG_SIZE,   OPT_REG_OFFSET  );
	init_option_stack(&walk_stack->options[OPT_ADDR],  args, base, OPT_ADDR_SIZE,  OPT_ADDR_OFFSET );
	init_option_stack(&walk_stack->options[OPT_STATE], args, base, OPT_STATE_SIZE, OPT_STATE_OFFSET);
}

static void init_option_stack(option_data_stack_s *opt_stack, char** args, uint32_t* base, uint32_t size, uint32_t start)
{
	opt_stack->args = args + start;
	opt_stack->base = base + start;

	for (uint32_t i = 0; i < size; ++i)
	{
		opt_stack->args[i] = NULL;
		opt_stack->base[i] = 10;
	}

	opt_stack->idx = 0;
}

static bool get_stack_data(walk_stack_s* stack, yacl_inout_data_s* inout_data)
{
	bool option_data_present = false;
    option_data_stack_s option;

    for (opt_stack_idx_e i = OPT_DATA; i < NUM_OPTIONS; ++i)
    {
        if (stack->valid_options[i] == false)
            continue;

        option = stack->options[i];

		switch (i)
		{
		case OPT_DATA:
			option_data_present = true;
			for (uint32_t j = 0; j < option.idx; ++j)
				inout_data->data[j] = (uint32_t)strtoull(option.args[j], &option.args[j] + strlen(option.args[j]), (int32_t)option.base[j]);
			break;

		case OPT_REG:
			option_data_present = true;
			for (uint32_t j = 0; j < option.idx; ++j)
				inout_data->reg[j] = (uint32_t)strtoull(option.args[j], &option.args[j] + strlen(option.args[j]), (int32_t)option.base[j]);
			break;

		case OPT_ADDR:
			option_data_present = true;
			inout_data->addr = (uint32_t)strtoull(option.args[0], &option.args[0] + strlen(option.args[0]), (int32_t)option.base[0]);
			break;

		case OPT_STATE:
			option_data_present = true;
			inout_data->state = (uint32_t)strtoull(option.args[0], &option.args[0] + strlen(option.args[0]), (int32_t)option.base[0]);
		}
    }
	return option_data_present;
}

void set_func(yacl_inout_data_s* prev_inout_data, yacl_inout_data_s* inout_data)
{
	memcpy(prev_inout_data->data, inout_data->data, OPT_DATA_SIZE * sizeof (uint32_t));
	memcpy(prev_inout_data->read_data, inout_data->read_data, OPT_DATA_SIZE * sizeof (uint32_t));
	memcpy(prev_inout_data->reg, inout_data->reg, OPT_REG_SIZE* sizeof (uint32_t));

	prev_inout_data->addr = inout_data->addr;
	prev_inout_data->state = inout_data->state;
}

static void help_func(cb_lut_t* cb_lut)
{
    yacl_printf("\n\rYACL Help\n\n\r\t<action> <stream> <options>\n\n\r");

    yacl_printf("\toptions\n\t-d\tdata to pass to callback function\n\r");
    yacl_printf("\t       \t-r\tspecific register values to pass to callback function\n\r");
    yacl_printf("\t       \t-s\tstream address to pass to callback function (eg. i2c)\n\r");
    yacl_printf("\t       \t-w\tconditional value to pass to callback function\n\r");

    yacl_printf("\n\tregistered callbacks (action, streams)\n\n\r");

    char* stream_names[NUM_STREAMS] = { "gpio", "i2c", "spi"};

    action_e action = ACTION_WRITE;
    stream_e stream = STREAM_GPIO;

    for ( ; action < NUM_USR_DEF_ACTIONS; ++action)
    {
        switch (action)
        {
            case ACTION_WRITE:
                yacl_printf("\twrite\t");

                for ( ; stream < NUM_STREAMS; ++stream)
                {
                    if (cb_lut->not_null_cbs[action][stream] == true)
                        yacl_printf("%s ", stream_names[stream]);
                }
                yacl_printf("\n");
                break;

            case ACTION_READ:
                yacl_printf("\tread\t");

                for ( ; stream < NUM_STREAMS; ++stream)
                {
                    if (cb_lut->not_null_cbs[action][stream] == true)
                        yacl_printf("%s ", stream_names[stream]);
                }
                yacl_printf("\n");
                break;

            case ACTION_PLOT:
                yacl_printf("\tplot\t");

                for ( ; stream < NUM_STREAMS; ++stream)
                {
                    if (cb_lut->not_null_cbs[action][stream] == true)
                        yacl_printf("%s ", stream_names[stream]);
                }
                yacl_printf("\n");
        }
        yacl_printf("\r");
        stream = STREAM_GPIO;
    }
    yacl_printf("\n\r");
}

static void clear_func()
{
    vt100_welcome();
}

static void init_graph(yacl_graph_s *usr_graph)
{
    if (!usr_graph)
        return;

    g_graph.units       = usr_graph->units;
    g_graph.num_samples = usr_graph->num_samples;
    g_graph.num_steps   = usr_graph->num_steps;
    g_graph.upper_range = usr_graph->upper_range;
    g_graph.lower_range = usr_graph->lower_range;
}

static yacl_error_e call_action_func(cb_lut_t* cb_lut, action_e action, stream_e stream, yacl_inout_data_s* inout_data, yacl_inout_data_s* prev_inout_data, bool use_prev_io_data)
{
	switch (action)
	{
	case ACTION_SET:
		set_func(prev_inout_data, inout_data);
		return YACL_SUCCESS;

	case ACTION_HELP:
		help_func(cb_lut);
		break;

	case ACTION_CLEAR:
		clear_func();
		break;

	case ACTION_PLOT:
		if (cb_lut->not_null_cbs[action][stream] == true)
		{
			is_plot = true;
			vt100_draw_graph(&g_graph);
		}
	case ACTION_WRITE:
	case ACTION_READ:
		if (cb_lut->not_null_cbs[action][stream] == true)
		{
			if (use_prev_io_data == true)
			{
				cb_lut->funcs[action][stream](inout_data);
				set_func(prev_inout_data, inout_data);
			}
			else
			{
				cb_lut->funcs[action][stream](prev_inout_data);
				set_func(inout_data, prev_inout_data);
			}
		}
		else
		{
			return YACL_UNKNOWN_CMD;
		}
		break;

	default:
		return YACL_UNKNOWN_CMD;
	}
    return YACL_SUCCESS;
}
