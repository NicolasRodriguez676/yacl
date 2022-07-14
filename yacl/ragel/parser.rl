#include <string.h>

#include "parser.h"

extern usr_printf_f yacl_printf;

%%{
    machine yacl_parser;

#*** YACL PARSER ACTIONS -----------------------------------------------------------------------------------------------

# YACL PARSER ACTIONS

    action write_ext { stack->action = ACTION_WRITE;}
    action read_ext  { stack->action = ACTION_READ; }
    action plot_ext  { stack->action = ACTION_PLOT; }
    action set_ext   { stack->action = ACTION_SET;  }
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

	action u_ent     {
							stack->valid_options[OPT_UPPER_BOUND] = true;
							option = &stack->options[OPT_UPPER_BOUND];
					 }

	action l_ent     {
							stack->valid_options[OPT_LOWER_BOUND] = true;
							option = &stack->options[OPT_LOWER_BOUND];
					 }

	action a_ent    {
							stack->valid_options[OPT_NUM_SAMPLES] = true;
							option = &stack->options[OPT_NUM_SAMPLES];
					 }

	action t_ent    {
							stack->valid_options[OPT_NUM_STEPS] = true;
							option = &stack->options[OPT_NUM_STEPS];
				    }

	action n_ent    {
							stack->valid_options[OPT_UNITS] = true;
							option = &stack->options[OPT_UNITS];
	                }

	action data_ent  {
	                    if (option->idx >= 32)
	                    {
		                    stack->bufr_err_idx = p - stack->bufr_begin;
	                        return YACL_TOO_MANY_ARGS;
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
	                            stack->bufr_err_idx = p - stack->bufr_begin;
                                return YACL_ARG_TOO_LARGE;
                            }

                            ++option->idx;
                        }
						
	action data_ext_float  {
								fc = '\0';
								++option->idx;
							}
	
	action data_ext_string  {
								fc = '\0';
		
								if (strlen(option->args[option->idx]) > GRAPH_UNITS_MAX_LEN)
								{
									stack->bufr_err_idx = p - stack->bufr_begin;
									return YACL_ARG_TOO_LONG;
								}
		
								++option->idx;
							}

    action xdata_ent {
                            option->base[option->idx] = 16;
                        }
						
	action space_ent {
							stack->bufr_err_beg = p - stack->bufr_begin;
						}
	
# YACL PARSER ERROR

    action error {
					    stack->bufr_err_idx = p - stack->bufr_begin;
                        return YACL_PARSE_BAD;
                    }


#*** YACL PARSER CONTROL --------------------------------------------------------------------------------------------

	only_space_1 = (" " >space_ent);
	only_space_01 = (" "? >space_ent);
	terminator = "\r";
    opt_prefix = only_space_01 . "-";
    hex_prefix = "H";

#*** YACL PARSER DATA -----------------------------------------------------------------------------------------------
    
    data        = ((hex_prefix %*xdata_ent) . (xdigit+ >data_ent %data_ext)) | (digit+ >data_ent %data_ext);
    array       = "," . only_space_01 . data . terminator?;
    data_single = data . (only_space_01 | terminator?);
    data_array  = data . terminator? . array+;

	data_single_float =  ((("-"? . digit+) >data_ent) . ("." . digit+)?  %data_ext_float);
	
	data_string = (alpha+ >data_ent %data_ext_string) . (only_space_01 | terminator?);
	
#*** YACL PARSER OPTIONS --------------------------------------------------------------------------------------------
    
    opt_d = "d" . only_space_1 . (data_single | data_array);
    opt_r = "r" . only_space_1 . (data_single | data_array);
    opt_s = "s" . only_space_1 . data_single;
    opt_w = "w" . only_space_1 . data_single;
    opt_u = "u" . only_space_1 . data_single_float;
    opt_l = "l" . only_space_1 . data_single_float;
    opt_a = "a" . only_space_1 . data_single;
    opt_t = "t" . only_space_1 . data_single;
    opt_n = "n" . only_space_1 . data_string;

    options = (opt_d >d_ent) |
              (opt_r >r_ent) |
              (opt_s >s_ent) |
              (opt_w >w_ent);
    
    option = opt_prefix . options;
    option_collect = option . ((option . terminator?){0,3} | terminator);

	option_set = opt_prefix . (options | (opt_u >u_ent) | (opt_l >l_ent) | (opt_a >a_ent) | (opt_t >t_ent) | (opt_n >n_ent));
    option_collect_set = option_set . ((option_set . terminator?){0,8} | terminator);
	

#*** YACL PARSER STREAMS --------------------------------------------------------------------------------------------

    stream_gpio = "gpio" %~gpio_ext;
    stream_i2c  = "i2c"  %~i2c_ext;
    stream_spi  = "spi"  %~spi_ext;

    streams = only_space_1 . (stream_gpio | stream_i2c | stream_spi) . (option_collect | terminator);


#*** YACL PARSER MACHINE --------------------------------------------------------------------------------------------

    str_write = ("write"  %~write_ext) .  streams;
    str_read  = ("read"   %~read_ext)  .  streams;
    str_plot  = ("plot"   %~plot_ext)  .  streams;
    str_set   = ("set"    %~set_ext)   .  option_collect_set . terminator?;
    str_help  = ("help"   %~help_ext)  .  terminator;
    str_clear = ("clear"  %~clear_ext) .  terminator;


    main := (
                str_write |
                str_read  |
                str_plot  |
                str_set   |
                str_help  |
                str_clear
            )
            $!error;

    write data;
}%%

yacl_error_e parser(char* p, char* pe, walk_stack_s* stack)
{
    int cs = 0;
    %% write init;

    stack->bufr_begin = p;
	stack->bufr_err_beg = 0;
	stack->bufr_err_idx = 0;
	
    option_data_stack_s* option;

    char* eof = pe;

    %% write exec;

    return YACL_SUCCESS;
}