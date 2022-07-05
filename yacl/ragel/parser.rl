#include <string.h>

#include "parser.h"

extern usr_printf_t yacl_printf;

%%{
    machine yacl_parser;

#*** YACL PARSER ACTIONS -----------------------------------------------------------------------------------------------

# YACL PARSER ACTIONS

    action write_ext { stack->action = ACTION_WRITE;}
    action read_ext  { stack->action = ACTION_READ; }
    action plot_ext  { stack->action = ACTION_PLOT; }
    action conf_ext  { stack->action = ACTION_CONF; }
    action help_ext  { stack->action = ACTION_HELP; }
    action clear_ext { stack->action = ACTION_CLEAR;}

# YACL PARSER STREAMS
        
    action gpio_ext { stack->stream = STREAM_GPIO; }
    action i2c_ext  { stack->stream = STREAM_I2C;  }
    action spi_ext  { stack->stream = STREAM_SPI;  }

# YACL PARSER OPTIONS

    action d_ent     {
                            stack->valid_options[OPT_DATA] = true;
                            option = &stack->options[OPT_DATA];
                        }

    action r_ent     {
                            stack->valid_options[OPT_REG] = true;
                            option = &stack->options[OPT_REG];
                        }

    action s_ent     {
                            stack->valid_options[OPT_ADDR] = true;
                            option = &stack->options[OPT_ADDR];
                        }

    action w_ent     {
                            stack->valid_options[OPT_STATE] = true;
                            option = &stack->options[OPT_STATE];
                        }

    action data_ent  {
                            if (option->idx >= 32)
                            { 
                                uint32_t bufr_idx = p - stack->bufr_begin;
                                yacl_printf("parse error at index %u. too many arguments\n", bufr_idx);
                                return YACL_PARSE_BAD;
                            }

                            option->args[option->idx] = p;
                        }

    action data_ext  {
                            fc = '\0'; 
                            
                            uint32_t max_str_len = 10;

                            if (option->base[option->idx] == 16)
                                max_str_len = 8;

                            if (strlen(option->args[option->idx]) > max_str_len)
                            { 
                                uint32_t bufr_idx = p - stack->bufr_begin;
                                yacl_printf("parse error at index %u. argument too large\n", bufr_idx);
                                return YACL_PARSE_BAD;
                            }

                            ++option->idx;
                        }

    action xdata_ent {
                            option->base[option->idx] = 16;
                        }

# YACL PARSER ERROR

    action error {
                        uint32_t bufr_idx = p - stack->bufr_begin;
                        yacl_printf("parse error at index %u. misspelled or unknown argument\n", bufr_idx);
                        return YACL_PARSE_BAD;
                    }


#*** YACL PARSER CONTROL --------------------------------------------------------------------------------------------

    terminator = ".";
    opt_prefix = " "? . "-";
    hex_prefix = "H";

#*** YACL PARSER DATA -----------------------------------------------------------------------------------------------
    
    data        = ((hex_prefix %*xdata_ent) . (xdigit+ >data_ent %data_ext)) | (digit+ >data_ent %data_ext);
    array       = ",". data . terminator?;
    data_single = data . (" "? | terminator?);
    data_array  = data . terminator? . array+;

#*** YACL PARSER OPTIONS --------------------------------------------------------------------------------------------
    
    opt_d = "d " . (data_single | data_array);
    opt_r = "r " . (data_single | data_array);
    opt_s = "s " . (data_single );
    opt_w = "w " . (data_single );

    options = (opt_d >d_ent) |
                 (opt_r >r_ent) |
                 (opt_s >s_ent) |
                 (opt_w >w_ent);
    
    option = opt_prefix . options;
    option_collect = option . ((option . terminator?){0,3} | terminator);


#*** YACL PARSER STREAMS --------------------------------------------------------------------------------------------

    stream_gpio = "gpio" %~gpio_ext;
    stream_i2c  = "i2c"  %~i2c_ext;
    stream_spi  = "spi"  %~spi_ext;

    streams = " " . (stream_gpio | stream_i2c | stream_spi) . (option_collect | terminator);


#*** YACL PARSER MACHINE --------------------------------------------------------------------------------------------

    str_write = ("write"  %~write_ext) .  streams;
    str_read  = ("read"   %~read_ext)  .  streams;
    str_plot  = ("plot"   %~plot_ext)  .  streams;
    str_conf  = ("conf"   %~conf_ext)  .  streams;
    str_help  = ("help"   %~help_ext)  .  terminator;
    str_clear = ("clear"  %~clear_ext) .  terminator;


    main := (
                str_write |
                str_read  |
                str_plot  |
                str_conf  |
                str_help  |
                str_clear
            )
            $!error;

    write data;
}%%

yacl_error_t parser(char* p, char* pe, walk_stack_s* stack)
{
    int cs = 0;
    %% write init;

    stack->bufr_begin = p;

    option_data_stack_s* option;

    char* eof = pe;

    %% write exec;

    return YACL_SUCCESS;
}