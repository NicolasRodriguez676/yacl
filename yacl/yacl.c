#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "yacl.h"
#include "yacl_types.h"

#include "parser/parser.h"
#include "vt100/vt100.h"

usr_printf_f yacl_printf;
usr_snprintf_f yacl_snprintf;

//      GLOBALS

static cb_lut_s g_cb_lut;
static yacl_inout_data_s g_prev_inout_data;

static ring_buffer_s g_input_bufr;
static user_input_string_s g_usr_input;

static bool g_is_plot;
static yacl_graph_s g_graph;

//      FUNCTION PROTOTYPES

static void init_input_bufr(ring_buffer_s* input_bufr);
static void init_usr_input_bufr(user_input_string_s* usr_input);
static void init_graph(yacl_graph_s* usr_graph);

static yacl_error_e preproc_input_bufr(ring_buffer_s* input_bufr, user_input_string_s* usr_input);
static void strip_spaces(user_input_string_s* usr_input);

static uint32_t get_bufr_unread(ring_buffer_s* input_bufr);
static void empty_input_bufr(ring_buffer_s* input_bufr);
static void empty_usr_str_bufr(user_input_string_s* usr_input);

static void init_cbs(cb_lut_s* cb_lut, yacl_usr_callbacks_s* usr_callbacks);
static void set_callbacks(cb_lut_s* cb_lut, void (** usr_cb)(yacl_inout_data_s*), stream_e stream);

static void init_walk_stack(walk_stack_s* walk_stack, option_data_stack_s* option_stack, char** args, uint32_t* base);
static void init_option_stack(option_data_stack_s* opt_stack, char** args, uint32_t* base, uint32_t size, uint32_t start);
static bool get_stack_data(walk_stack_s* stack, yacl_inout_data_s* inout_data, yacl_graph_s* graph);

static void set_func(yacl_inout_data_s* prev_inout_data, yacl_inout_data_s* inout_data);
static void help_func(cb_lut_s* cb_lut);
static void clear_func(user_input_string_s* usr_input);

static yacl_error_e call_action_func(cb_lut_s* cb_lut, walk_stack_s* walk, user_input_string_s* usr_input, yacl_inout_data_s* inout_data, yacl_inout_data_s* prev_inout_data, bool use_prev_io_data);

//      PUBLIC      ****************************************************************************************************

void yacl_init(yacl_usr_callbacks_s* usr_callbacks, yacl_graph_s* usr_graph)
{
	// assume user provides valid printf functions
	yacl_printf = usr_callbacks->usr_print_funcs.usr_printf;
	yacl_snprintf = NULL; // usr_callbacks->usr_print_funcs.usr_snprintf;
	
	// initialize callbacks into protocol lookup table
	// record non-null callbacks as valid
	init_cbs(&g_cb_lut, usr_callbacks);
	
	// initialize buffers
	init_input_bufr(&g_input_bufr);
	init_usr_input_bufr(&g_usr_input);
	
	// set up the graph properties for plotting
	g_is_plot = false;
	init_graph(usr_graph);
	
	vt100_welcome();
}

void yacl_wr_buf(char data)
{
	if (g_is_plot)
	{
		if (data == EXIT_PLOT)
			g_is_plot = false;
		return;
	}
	
	if ((g_input_bufr.head & INPUT_BUFR_MASK) == ((g_input_bufr.tail & INPUT_BUFR_MASK) - 1))
	{
		g_input_bufr.overrun = true;
		return;
	}
	else
	{
		g_input_bufr.bufr[g_input_bufr.head++ & INPUT_BUFR_MASK] = data;
	}
}

yacl_error_e yacl_parse_cmd()
{
	yacl_error_e error = preproc_input_bufr(&g_input_bufr, &g_usr_input);
	
	if (error != YACL_SUCCESS)
	{
		if ( error == YACL_USR_INPUT_FULL)
		{
			yacl_printf("\n\r%s", yacl_error_desc(YACL_USR_INPUT_FULL));
			empty_input_bufr(&g_input_bufr);
			empty_usr_str_bufr(&g_usr_input);
			vt100_yacl_view();
		}
		return error;
	}
	
	// ignore enter 'spam'
	if (g_usr_input.bufr[0] == ASCII_CR)
	{
		vt100_yacl_view();
		empty_input_bufr(&g_input_bufr);
		empty_usr_str_bufr(&g_usr_input);
		return YACL_NO_CMD;
	}
	
	strip_spaces(&g_usr_input);
	
	char* stack_args[OPT_STACK_SIZE];
	uint32_t stack_base[OPT_STACK_SIZE];
	option_data_stack_s option_stack[NUM_OPTIONS];
	
	walk_stack_s stack;
	init_walk_stack(&stack, option_stack, stack_args, stack_base);
	
	error = parser((char*)g_usr_input.bufr, (char*)g_usr_input.bufr + strlen((char*)g_usr_input.bufr), &stack);
	
	if (error != YACL_SUCCESS)
	{
		vt100_report_exact_error(&stack, error);
		vt100_yacl_view();
		empty_usr_str_bufr(&g_usr_input);
		return error;
	}
	
	yacl_inout_data_s inout_data;
	bool data_present = get_stack_data(&stack, &inout_data, &g_graph);

	error = call_action_func(&g_cb_lut, &stack, &g_usr_input, &inout_data, &g_prev_inout_data, data_present);

	if (error != YACL_SUCCESS)
		yacl_printf("\n\r%s", yacl_error_desc(error));
	
	vt100_yacl_view();
	empty_usr_str_bufr(&g_usr_input);
	return error;
}

yacl_error_e yacl_plot(float data)
{
	// yacl_wr_bufr can change `g_is_plot`
	if (g_is_plot)
	{
		vt100_plot_graph(&g_graph, data);
		return YACL_SUCCESS;
	}
	else
	{
		return YACL_NO_CMD;
	}
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
		{ YACL_SUCCESS,         "no errors. good to go"                             },
		{ YACL_UNKNOWN_CMD,     "command was not found in given commands"           },
		{ YACL_NO_CMD,          "command incomplete"                                },
		{ YACL_USR_INPUT_FULL,  "user input buffer full. restarting"                },
		{ YACL_NO_CALLBACK,     "command was not given a callback function"         },
		{ YACL_TOO_MANY_ARGS,   "error parsing. too many arguments for option"      },
		{ YACL_ARG_TOO_LARGE,   "error parsing. argument numerical value too large" },
		{ YACL_ARG_TOO_LONG,    "error parsing. unit string length max is 8"        },
		{ YACL_PARSE_BAD,       "error parsing. misspelled or unknown argument"     }
	};
	
	return error_desc[error].msg;
}

//      PRIVATE     ****************************************************************************************************

static yacl_error_e preproc_input_bufr(ring_buffer_s* input_bufr, user_input_string_s* usr_input)
{
	uint32_t num_new_bytes = get_bufr_unread(input_bufr);
	
	if (num_new_bytes < 1)
		return YACL_NO_CMD;
	
	uint8_t data;
	bool is_seq = false;
	bool is_esc = false;
	
	while (num_new_bytes--)
	{
		data = input_bufr->bufr[input_bufr->tail++ & INPUT_BUFR_MASK];
		
		switch (data)
		{
		case ASCII_CR:
			usr_input->bufr[usr_input->index++] = data;
			return YACL_SUCCESS;
			
		// begin vt100 escape sequence
		// must have all bytes of escape sequence in input buffer to properly work
		case TERM_ESC:
			is_esc = true;
			break;
		
		case TERM_CSI:
			if (is_esc == true)
			{
				is_esc = false;
				is_seq = true;
			}
			break;
		
		case TERM_CSR_LEFT:
			if (is_seq == false)
				break;
			
			// prevent cursor moving into yacl view on screen
			// move user string buffer cursor left
			if (usr_input->cursor == 0)
				vt100_cursor_forward(1);
			else if (usr_input->cursor > 0)
				--usr_input->cursor;
			
			is_seq = false;
			break;
		
		case TERM_CSR_RIGHT:
			if (is_seq == false)
				break;
			
			// prevent cursor moving past user string buffer index
			// move buffer cursor right, if not at user string buffer index
			if (usr_input->index == usr_input->cursor)
				vt100_cursor_backward(1);
			else
				++usr_input->cursor;
			
			is_seq = false;
			break;
		
		case TERM_BACKSPACE:
			// prevent backspacing into yacl view
			if (usr_input->index == 0 && usr_input->bufr[usr_input->index] == '\0')
				break;
			
			vt100_backspace();
			
			--usr_input->index;
			--usr_input->cursor;
			
			// if the cursor is not at the index, then backspacing needs to copy user string buffer back by one byte
			if ((usr_input->index + 1) > (usr_input->cursor + 1))
				memmove(usr_input->bufr + usr_input->cursor, usr_input->bufr + (usr_input->cursor + 1), usr_input->index - usr_input->cursor);

			// erase data after being shifted left by one byte
			// does not matter if backspace happens before index, the whole user string buffer shrinks by one byte
			usr_input->bufr[usr_input->index] = '\0';
			break;
		
		case '\\':
			// internal status message meant for debugging. may use for displaying inout_data
			// as of this implementation, target MCU echo is used. does not echo back this character
			vt100_cursor_save();
			yacl_printf("\x1b[50;1f\x1b[2K\x1b[47m\x1b[30m"
						"STATUS index=\x1b[36m%u\x1b[30m, "
						"pos=\x1b[36m%u\x1b[30m, "
						"step=\x1b[36m%u\x1b[30m, "
						"sample=\x1b[36m%u\x1b[30m, "
						"upper=\x1b[36m%.3f\x1b[30m, "
						"lower=\x1b[36m%.3f\x1b[30m, "
						"units=\"\x1b[36m%s\x1b[30m\", "
						"bufr=\"\x1b[36m%s\x1b[30m\""
						"\x1b[0m", usr_input->index, usr_input->cursor, g_graph.num_steps, g_graph.num_samples, g_graph.upper_range, g_graph.lower_range, g_graph.units, usr_input->bufr);
			
			vt100_cursor_restore();
			break;
		
		default:
			// potential issue. should not be needed
			// is_esc = false;
			
			// last entry must be a carriage return and null terminated
			if (usr_input->index == (USER_INPUT_LEN - 2))
				return YACL_USR_INPUT_FULL;
			
			// if cursor is not at index. writing to user string buffer will
			// copy the user string buffer one byte to the right, from the cursor
			// refresh yacl view and user string with injected character on the display
			if (usr_input->index > usr_input->cursor)
			{
				memmove(usr_input->bufr + usr_input->cursor, usr_input->bufr + (usr_input->cursor - 1), usr_input->index - (usr_input->cursor - 1));
				
				usr_input->bufr[usr_input->cursor++] = data;
				++usr_input->index;
				
				vt100_cursor_save();
				vt100_erase_current_line();
				yacl_printf("\r>> %s", usr_input->bufr);
				vt100_cursor_restore();
			}
			else
			{
				++usr_input->cursor;
				usr_input->bufr[usr_input->index++] = data;
			}
		}
	}
	return YACL_NO_CMD;
}

static uint32_t get_bufr_unread(ring_buffer_s* input_bufr)
{
	if (input_bufr->overrun == true)
	{
		yacl_printf("\n\rOVERRUN!\n\r>> ");
		empty_input_bufr(input_bufr);
		return 0;
	}
	
	return input_bufr->head - input_bufr->tail;
}

static void empty_input_bufr(ring_buffer_s* input_bufr)
{
	memset(input_bufr->bufr, 0, INPUT_DATA_LEN);
	
	input_bufr->head = 0;
	input_bufr->tail = 0;
	input_bufr->overrun = false;
}

static void empty_usr_str_bufr(user_input_string_s* usr_input)
{
	memset(usr_input->bufr, 0, USER_INPUT_LEN);
	
	usr_input->index = 0;
	usr_input->cursor = 0;
}

static void init_cbs(cb_lut_s* cb_lut, yacl_usr_callbacks_s* usr_callbacks)
{
	yacl_cb_funcs_f usr_cb[NUM_STREAMS];
	
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

static void set_callbacks(cb_lut_s* cb_lut, void (** usr_cb)(yacl_inout_data_s*), stream_e stream)
{
	for (action_e action = ACTION_NONE; action < NUM_USR_DEF_ACTIONS; ++action)
	{
		if (usr_cb[action] != NULL)
			cb_lut->funcs[action][stream] = usr_cb[action];
	}
}

static void init_walk_stack(walk_stack_s* walk_stack, option_data_stack_s* option_stack, char** args, uint32_t* base)
{
	walk_stack->action = ACTION_NONE;
	walk_stack->stream = STREAM_NONE;
	
	for (option_stack_index_e i = OPT_NONE; i < NUM_OPTIONS; ++i)
	{
		walk_stack->valid_options[i] = false;
		walk_stack->options[i] = option_stack[i];
	}
	
	init_option_stack(&walk_stack->options[OPT_DATA],  args, base, OPT_DATA_SIZE,  OPT_DATA_OFFSET );
	init_option_stack(&walk_stack->options[OPT_REG],   args, base, OPT_REG_SIZE,   OPT_REG_OFFSET  );
	init_option_stack(&walk_stack->options[OPT_ADDR],  args, base, OPT_ADDR_SIZE,  OPT_ADDR_OFFSET );
	init_option_stack(&walk_stack->options[OPT_STATE], args, base, OPT_STATE_SIZE, OPT_STATE_OFFSET);
	
	init_option_stack(&walk_stack->options[OPT_UPPER_BOUND], args, base, OPT_UPPER_SIZE, OPT_UPPER_OFFSET);
	init_option_stack(&walk_stack->options[OPT_LOWER_BOUND], args, base, OPT_LOWER_SIZE, OPT_LOWER_OFFSET);
	init_option_stack(&walk_stack->options[OPT_NUM_SAMPLES], args, base, OPT_NSMPL_SIZE, OPT_NSMPL_OFFSET);
	init_option_stack(&walk_stack->options[OPT_NUM_STEPS],   args, base, OPT_NSTPS_SIZE, OPT_NSTPS_OFFSET);
	init_option_stack(&walk_stack->options[OPT_UNITS],       args, base, OPT_UNITS_SIZE, OPT_UNITS_OFFSET);
}

static void init_option_stack(option_data_stack_s* opt_stack, char** args, uint32_t* base, uint32_t size, uint32_t start)
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

static bool get_stack_data(walk_stack_s* stack, yacl_inout_data_s* inout_data, yacl_graph_s* graph)
{
	bool option_data_present = false;
	option_data_stack_s* option;
	
	for (option_stack_index_e i = OPT_NONE; i < NUM_OPTIONS; ++i)
	{
		if (stack->valid_options[i] == false)
			continue;
		
		option = &stack->options[i];
		option_data_present = true;
		
		switch (i)
		{
		case OPT_DATA:
			for (uint32_t j = 0; j < option->idx; ++j)
				inout_data->data[j] = (uint32_t)strtoull(option->args[j], &option->args[j] + strlen(option->args[j]), (int32_t)option->base[j]);
			break;
		
		case OPT_REG:
			for (uint32_t j = 0; j < option->idx; ++j)
				inout_data->reg[j] = (uint32_t)strtoull(option->args[j], &option->args[j] + strlen(option->args[j]), (int32_t)option->base[j]);
			break;
		
		case OPT_ADDR:
			inout_data->addr = (uint32_t)strtoull(option->args[OPT_NONE], &option->args[OPT_NONE] + strlen(option->args[OPT_NONE]), (int32_t)option->base[OPT_NONE]);
			break;
		
		case OPT_STATE:
			inout_data->state = (uint32_t)strtoull(option->args[OPT_NONE], &option->args[OPT_NONE] + strlen(option->args[OPT_NONE]), (int32_t)option->base[OPT_NONE]);
			break;
			
		case OPT_UPPER_BOUND:
			graph->upper_range = strtof(option->args[OPT_NONE], &option->args[OPT_NONE] + strlen(option->args[OPT_NONE]));
			break;
			
		case OPT_LOWER_BOUND:
			graph->lower_range = strtof(option->args[OPT_NONE], &option->args[OPT_NONE] + strlen(option->args[OPT_NONE]));
			break;
		
		case OPT_NUM_SAMPLES:
			graph->num_samples = 0xff & strtoull(option->args[OPT_NONE], &option->args[OPT_NONE] + strlen(option->args[OPT_NONE]), (int32_t)option->base[OPT_NONE]);
			break;
		
		case OPT_NUM_STEPS:
			graph->num_steps = 0xffff & strtoull(option->args[OPT_NONE], &option->args[OPT_NONE] + strlen(option->args[OPT_NONE]), (int32_t)option->base[OPT_NONE]);
			break;
			
		case OPT_UNITS:
			memcpy(graph->units, option->args[OPT_NONE], GRAPH_UNITS_MAX_LEN);
			break;
			
		default:
			// unreachable
			break;
		}
	}
	return option_data_present;
}

void set_func(yacl_inout_data_s* prev_inout_data, yacl_inout_data_s* inout_data)
{
	memcpy(prev_inout_data->data, inout_data->data, OPT_DATA_SIZE * sizeof(uint32_t));
	memcpy(prev_inout_data->read_data, inout_data->read_data, OPT_DATA_SIZE * sizeof(uint32_t));
	memcpy(prev_inout_data->reg, inout_data->reg, OPT_REG_SIZE * sizeof(uint32_t));
	
	prev_inout_data->addr = inout_data->addr;
	prev_inout_data->state = inout_data->state;
}

static void help_func(cb_lut_s* cb_lut)
{
	yacl_printf("\n\rYACL Help\n\n\r\t<action> <stream> <options>\n\n\r"
	            "\toptions\n\t-d\tdata to pass to callback function\n\r"
	            "\t       \t-r\tspecific register values to pass to callback function\n\r"
	            "\t       \t-s\tstream address to pass to callback function (eg. i2c)\n\r"
	            "\t       \t-w\tconditional value to pass to callback function (sWitch-case)\n\r"
	            "\noptions arguments are optional. the previously defined (or default values) may be passed by omitting options\n\r"
	            "\n\t<action> <stream>\n\r"
	            "\nthe above options are also accessible by using the 'set' action\n\r"
	            "\n\tset <options>\n\r"
	            "\nnote that the same data is passed to each callback stream, which is both advantageous and restrictive\n\r"
	            "\n\tregistered callbacks (action, streams)\n\n\r");
	
	char* stream_names[NUM_STREAMS] = { "gpio", "i2c", "spi" };
	char* action_names[NUM_USR_DEF_ACTIONS] = { "write", "read", "plot" };
	
	for (action_e action = ACTION_NONE; action < NUM_USR_DEF_ACTIONS; ++action)
	{
		yacl_printf("\t%s\t", action_names[action]);
		
		for (stream_e stream = STREAM_NONE ; stream < NUM_STREAMS; ++stream)
		{
			if (cb_lut->funcs[action][stream] != NULL)
				yacl_printf("%s ", stream_names[stream]);
		}
		yacl_printf("\n\r");
	}
	yacl_printf("\n\r");
}

static void clear_func(user_input_string_s* usr_input)
{
	empty_usr_str_bufr(usr_input);
	vt100_welcome();
	vt100_cursor_upward(1);
}

static void init_input_bufr(ring_buffer_s* input_bufr)
{
	input_bufr->head = 0;
	input_bufr->tail = 0;
	input_bufr->overrun = false;
	
	memset(input_bufr->bufr, 0, INPUT_DATA_LEN);
}

static void init_usr_input_bufr(user_input_string_s* usr_input)
{
	usr_input->index = 0;
	usr_input->cursor = 0;
	
	memset(usr_input->bufr, 0, USER_INPUT_LEN);
}

static void init_graph(yacl_graph_s* usr_graph)
{
	if (usr_graph == NULL)
	{
		g_graph.upper_range = 1.0f;
		g_graph.lower_range = 0.0f;
		g_graph.num_steps   = 11;
		g_graph.num_samples = 60;
		g_graph.units[0] = 'V';
	}
	else
	{
		g_graph.num_samples = usr_graph->num_samples;
		g_graph.num_steps   = usr_graph->num_steps;
		g_graph.upper_range = usr_graph->upper_range;
		g_graph.lower_range = usr_graph->lower_range;
		
		memcpy(g_graph.units, usr_graph->units, GRAPH_UNITS_MAX_LEN);
	}
	
	g_graph.units[GRAPH_UNITS_MAX_LEN] = '\0';  // must be null terminated
}

static yacl_error_e call_action_func(cb_lut_s* cb_lut, walk_stack_s* walk, user_input_string_s* usr_input, yacl_inout_data_s* inout_data, yacl_inout_data_s* prev_inout_data, bool use_prev_io_data)
{
	action_e action = walk->action;
	stream_e stream = walk->stream;
	
	switch (action)
	{
	case ACTION_SET:
		set_func(prev_inout_data, inout_data);
		return YACL_SUCCESS;
	
	case ACTION_HELP:
		help_func(cb_lut);
		break;
	
	case ACTION_CLEAR:
		clear_func(usr_input);
		break;
	
	case ACTION_PLOT:
		if (cb_lut->funcs[action][stream] != NULL)
		{
			g_is_plot = true;
			vt100_draw_graph(&g_graph);
			
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
			
			vt100_end_plot(&g_graph);
		}
		else
		{
			return YACL_UNKNOWN_CMD;
		}
		break;
		
	case ACTION_WRITE:
	case ACTION_READ:
		if (cb_lut->funcs[action][stream] != NULL)
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
		// presumably unreachable
		return YACL_UNKNOWN_CMD;
	}
	return YACL_SUCCESS;
}

static void strip_spaces(user_input_string_s* usr_input)
{
	// this function makes the parser grammar simpler but ultimately ensures the user
	// can navigate redundant spaces on display that directly represent the user string buffer
	// other solutions are more complicated which require more memory, time and needlessly
	// complicates this program, which does not even do that much
	
	uint32_t bufr_str_len = strlen((char*)usr_input->bufr);
	uint32_t num_spaces = 0;
	
	char prev_char = ASCII_SP;
	char curr_char = '\0';
	
	for (uint32_t i = 0; i < bufr_str_len && (curr_char != ASCII_CR); ++i)
	{
		curr_char = usr_input->bufr[i];
		
		if (prev_char == ASCII_SP && curr_char == ASCII_SP)
		{
			while (true)
			{
				if (usr_input->bufr[i + num_spaces] != ASCII_SP)
					break;

				++num_spaces;
			}
			
			memmove(usr_input->bufr + i, usr_input->bufr + i + num_spaces, bufr_str_len - i + num_spaces);
			
			i += num_spaces;
			num_spaces = 0;
			curr_char = usr_input->bufr[i];
		}
		prev_char = curr_char;
	}
}
