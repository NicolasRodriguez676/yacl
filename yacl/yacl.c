#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#include "yacl.h"
#include "yacl_types.h"

#include "vt100/vt100.h"

//      GLOBALS

usr_printf_t yacl_printf;
usr_snprintf_t yacl_snprintf;

static protocol_lut_cb_t g_cmd_cbs = {
        .protocols = { "gpio", "i2c", "spi", "help" },
        .actions = { "read", "write", "plot" },
        .funcs = { { NULL, NULL } },
        .not_null_cbs = { false },
        .num_not_null_cbs = 0
};

static ring_buffer_t g_input_bufr = { .bufr = { 0 }, .head = 0, .tail = 0 };
static data_buffer_t g_tok_bufr = { .bufr = { 0 }, .idx = 0, .tok_array= { NULL }, .tok_cnt = 0 };

static bool input_bufr_ok = true;
static bool is_plot       = false;

static yacl_graph_t g_graph = {
        .upper_range = 1.0f,
        .lower_range = 0.0f,
        .num_steps   = 11,
        .num_samples = 60,
        .units       = "V",
};

//      FUNCTION PROTOTYPES

static yacl_error_t proc_in_bufr();
static yacl_error_t get_protocol_lut_idxs(uint32_t* protocol_idx, uint32_t* action_idx);
static bool         compare_tokens(const char *str_lhs, const char *str_rhs);

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
static uint32_t     get_prefix(const uint8_t* num_str);
static yacl_error_t get_reg_range(yacl_inout_data_t* inout_data);

static void         help_func(yacl_inout_data_t* inout_data);
static void         init_cbs(yacl_usr_callbacks_t* usr_callbacks);

static void         init_graph(yacl_graph_t* usr_graph);

/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

static void init_walk_stack(walk_stack_s *walk_stack, option_data_stack_s *option_stack, char **args, uint32_t *base);
static void init_option_stack(option_data_stack_s *opt_stack, char** args, uint32_t* base, uint32_t size, uint32_t start);

/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

//      PUBLIC      ****************************************************************************************************

void yacl_init(yacl_usr_callbacks_t *usr_callbacks, yacl_graph_t *usr_graph)
{
    // assume user provides valid printf functions
    yacl_printf = usr_callbacks->usr_print_funcs.usr_printf;
    yacl_snprintf = usr_callbacks->usr_print_funcs.usr_snprintf;

    // initialize callbacks into protocol lookup table
    // record non-null callbacks as valid
    init_cbs(usr_callbacks);

    // allocate token space in static global buffer
    // easier buffer + token management
    for (uint32_t i = 0; i < MAX_TOKENS; ++i)
        g_tok_bufr.tok_array[i] = g_tok_bufr.bufr + (MAX_TOKEN_LEN * i);

    // set up the graph properties for plotting
    init_graph(usr_graph);

    // clear the VT100 terminal screen
    vt100_rst_term();

    // display welcome message
    yacl_printf("YACL by Nick\n\n\rExplore peripherals connected to your MCU freely!\n\rType 'help' for more information or visit my GitHub\n\n\r>> ");
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

yacl_error_t yacl_parse_cmd()
{
    /*//////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
    if (g_input_bufr.bufr[(g_input_bufr.head - 1) & 0x7f] != '\n')
        return YACL_NO_CMD;

    char* stack_args[OPT_DATA_SIZE + OPT_REG_SIZE + OPT_ADDR_SIZE + OPT_STATE_SIZE];
    uint32_t stack_base[OPT_DATA_SIZE + OPT_REG_SIZE + OPT_ADDR_SIZE + OPT_STATE_SIZE];
    option_data_stack_s option_stack[NUM_OPTIONS];

    walk_stack_s stack;
    init_walk_stack(&stack, option_stack, stack_args, stack_base);

    if (!yaclG_parse((char*)g_input_bufr.bufr, (char*)g_input_bufr.bufr + g_input_bufr.head, &stack))
    {
        vt100_yacl_view();
        empty_bufr();
        return YACL_UNKNOWN_CMD;
    }

    yacl_inout_data_t inout_data;
    option_data_stack_s option;

    for (uint32_t i = 0; i < NUM_OPTIONS; ++i)
    {
        if (stack.valid_options[i] == false)
            continue;

        option = stack.options[i];

        if (i == OPT_DATA)
        {
            for (uint32_t j = 0; j < option.idx; ++j)
                inout_data.yP_data[j] = (uint32_t) strtoull(option.args[j], &option.args[j] + strlen(option.args[j]), (int32_t) option.base[j]);
        }
        else if (i == OPT_REG)
        {
            for (uint32_t j = 0; j < option.idx; ++j)
                inout_data.yP_reg[j] = (uint32_t) strtoull(option.args[j], &option.args[j] + strlen(option.args[j]), (int32_t) option.base[j]);
        }
        else if (i == OPT_ADDR)
        {
            inout_data.yP_addr = (uint32_t) strtoull(option.args[0], &option.args[0] + strlen(option.args[0]), (int32_t) option.base[0]);
        }
        else if (i == OPT_STATE)
        {
            inout_data.yP_state = (uint32_t) strtoull(option.args[0], &option.args[0] + strlen(option.args[0]), (int32_t) option.base[0]);
        }
    }

    if (stack.action == ACTION_HELP)
    {
        g_cmd_cbs.funcs[HELP_CB_IDX][0](&inout_data);

        empty_bufr();
        vt100_yacl_view();
        return YACL_SUCCESS;
    }

    act_flag_e action = stack.action - 1;
    str_flag_e stream = stack.stream - 1;

    if (g_cmd_cbs.not_null_cbs[stream] == true)
    {
        // verify register range for static inout_data_t buffer
        // action index for read is zero and write is one
        // OR allows bypassing inout buffer range, while still
        // calculating range for write operation
        if (action == PLOT_CB_IDX)
        {
            is_plot = true;
            vt100_draw_graph(&g_graph);
        }
            g_cmd_cbs.funcs[stream][action](&inout_data);
    }
    else
    {
        empty_bufr();
        vt100_error(yacl_error_desc(YACL_NO_CALLBACK));

        return YACL_UNKNOWN_CMD;
    }

    empty_bufr();
    vt100_yacl_view();
    return YACL_SUCCESS;

    /*//////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
}

yacl_error_t yacl_plot(float data)
{
    if (is_plot)
    {
        vt100_plot_graph(&g_graph, data);
        return YACL_SUCCESS;
    }
    else
        return YACL_NO_CMD;
}

void yacl_set_cb_null(yacl_usr_callbacks_t* usr_callbacks)
{
    // help users choose only what they want in their build of this program
    // reduces upkeep in their program and visual clutter

    usr_callbacks->usr_gpio_read = NULL;
    usr_callbacks->usr_gpio_write = NULL;
    usr_callbacks->usr_gpio_plot = NULL;

    usr_callbacks->usr_i2c_read = NULL;
    usr_callbacks->usr_i2c_write = NULL;
    usr_callbacks->usr_i2c_plot = NULL;

    usr_callbacks->usr_spi_read = NULL;
    usr_callbacks->usr_spi_write = NULL;
    usr_callbacks->usr_spi_plot = NULL;
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
            { YACL_NO_CALLBACK,    "command was not given a callback function" },
    };

    return error_desc[error].msg;
}

//      PRIVATE     ****************************************************************************************************

static yacl_error_t proc_in_bufr()
{
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

    // find index of protocol functions that matches protocol argument
    for ( ; *protocol_idx < NUM_PROTOCOLS; ++*protocol_idx)
    {
        token_is_valid = compare_tokens(g_cmd_cbs.protocols[*protocol_idx],(char *) g_tok_bufr.tok_array[ARG_PROTOCOL_IDX]);

        if (token_is_valid)
            break;
    }

    // end early if protocol index is equal to help index
    // only one argument should be typed for the help command
    if (!token_is_valid && *protocol_idx != HELP_CB_IDX)
        return YACL_UNKNOWN_CMD;
    else if (*protocol_idx == HELP_CB_IDX)
        return YACL_SUCCESS;

    // find index of action function that matches action argument
    for ( ; *action_idx < NUM_ACTIONS; ++*action_idx)
    {
        token_is_valid = compare_tokens(g_cmd_cbs.actions[*action_idx], (char *) g_tok_bufr.tok_array[ARG_ACTION_IDX]);

        if (token_is_valid)
            break;
    }

    if (!token_is_valid)
        return YACL_UNKNOWN_CMD;

    return YACL_SUCCESS;
}

static bool compare_tokens(const char *str_lhs, const char *str_rhs)
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
    input_bufr_ok = true;

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
    g_tok_bufr.tok_array[g_tok_bufr.tok_cnt][TOKENS_LEN_IDX] = g_tok_bufr.idx + 1;
    g_tok_bufr.tok_array[g_tok_bufr.tok_cnt][g_tok_bufr.idx] = '\0';
}

static bool wrt_to_token(uint8_t data)
{
    // index starts at 0 + null + length == 3
    if (g_tok_bufr.idx == MAX_TOKEN_LEN - 3)
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
    else if (action == WRITE_CB_IDX)
    {
        if (g_tok_bufr.tok_cnt >= TOKEN_CNT_WRITE_MIN)
            return true;
        else
            return false;
    }
    else // if (action == PLOT_CB_IDX)
    {
        if (g_tok_bufr.tok_cnt >= TOKEN_CNT_PLOT_MIN)
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
    else if (action == WRITE_CB_IDX)
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
    else // if (action == PLOT_CB_IDX)
    {
        inout_data->end_reg = inout_data->beg_reg;
        return true;
    }
}

static bool get_str_value(uint32_t* data, uint32_t tok_idx)
{
    uint32_t base = get_prefix(g_tok_bufr.tok_array[tok_idx]);
    char* end_ptr = (char*)(g_tok_bufr.tok_array[tok_idx] + g_tok_bufr.tok_array[tok_idx][TOKENS_LEN_IDX]);

    uint8_t i = 0;

    if (base == 16)
        i = 2;

    for ( ; i < g_tok_bufr.tok_array[tok_idx][TOKENS_LEN_IDX] - 1; ++i)
    {
        if (!isxdigit(g_tok_bufr.tok_array[tok_idx][i]))
        {
            empty_bufrs();
            vt100_error_data(yacl_error_desc(YACL_INVALID_ARG), tok_idx + 1);

            return false;
        }
    }
    *data = (uint32_t)strtoull((char*)g_tok_bufr.tok_array[tok_idx], &end_ptr, (int32_t)base);

    return true;
}

static yacl_error_t get_reg_range(yacl_inout_data_t* inout_data)
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

static uint32_t get_prefix(const uint8_t* num_str)
{
    if (num_str[0] == '0' && (num_str[1] == 'x' || num_str[1] == 'X') )
        return 16;
    else
        return 10;
}

static void init_cbs(yacl_usr_callbacks_t* usr_callbacks)
{
    g_cmd_cbs.funcs[HELP_CB_IDX][0] = help_func;
//	g_cmd_cbs.funcs[HELP_CB_IDX][1] = help_func;

    if (usr_callbacks->usr_gpio_read && usr_callbacks->usr_gpio_write && usr_callbacks->usr_gpio_plot)
    {
        g_cmd_cbs.funcs[GPIO_CB_IDX][READ_CB_IDX] = usr_callbacks->usr_gpio_read;
        g_cmd_cbs.funcs[GPIO_CB_IDX][WRITE_CB_IDX] = usr_callbacks->usr_gpio_write;
        g_cmd_cbs.funcs[GPIO_CB_IDX][PLOT_CB_IDX] = usr_callbacks->usr_gpio_plot;

        g_cmd_cbs.not_null_cbs[GPIO_CB_IDX] = true;
        ++g_cmd_cbs.num_not_null_cbs;
    }

    if (usr_callbacks->usr_i2c_read && usr_callbacks->usr_i2c_write && usr_callbacks->usr_i2c_plot)
    {
        g_cmd_cbs.funcs[I2C_CB_IDX][READ_CB_IDX] = usr_callbacks->usr_i2c_read;
        g_cmd_cbs.funcs[I2C_CB_IDX][WRITE_CB_IDX] = usr_callbacks->usr_i2c_write;
        g_cmd_cbs.funcs[I2C_CB_IDX][PLOT_CB_IDX] = usr_callbacks->usr_i2c_plot;

        g_cmd_cbs.not_null_cbs[I2C_CB_IDX] = true;
        ++g_cmd_cbs.num_not_null_cbs;
    }

    if (usr_callbacks->usr_spi_read && usr_callbacks->usr_spi_write && usr_callbacks->usr_spi_plot)
    {
        g_cmd_cbs.funcs[SPI_CB_IDX][READ_CB_IDX] = usr_callbacks->usr_spi_read;
        g_cmd_cbs.funcs[SPI_CB_IDX][WRITE_CB_IDX] = usr_callbacks->usr_spi_write;
        g_cmd_cbs.funcs[SPI_CB_IDX][PLOT_CB_IDX] = usr_callbacks->usr_spi_plot;

        g_cmd_cbs.not_null_cbs[SPI_CB_IDX] = true;
        ++g_cmd_cbs.num_not_null_cbs;
    }
}

static void help_func(yacl_inout_data_t* inout_data)
{
    yacl_printf("\n\rYACL Help\n\n\r\t<protocol> <action> <addr> <reg> [optional reg end] [data]\n\n\r");

    for (uint32_t i = 0; i < NUM_PROTOCOLS; ++i)
    {
        if (g_cmd_cbs.not_null_cbs[i])
            yacl_printf("%s :: read + write + plot\n\r", g_cmd_cbs.protocols[i]);
    }

    yacl_printf("\n\r");
}

void init_graph(yacl_graph_t *usr_graph)
{
    if (!usr_graph)
        return;

    g_graph.units       = usr_graph->units;
    g_graph.num_samples = usr_graph->num_samples;
    g_graph.num_steps   = usr_graph->num_steps;
    g_graph.upper_range = usr_graph->upper_range;
    g_graph.lower_range = usr_graph->lower_range;
}

/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

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

/*////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
