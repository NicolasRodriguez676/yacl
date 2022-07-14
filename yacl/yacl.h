#ifndef _YACL_H_
#define _YACL_H_

#include <stddef.h>
#include <stdint.h>

enum Yacl_Misc {
	// misc value
	// is present in parser.rl and yacl.c
    GRAPH_UNITS_MAX_LEN = 8,
	
	// Used internally. Also needed in this file
	// Option_Stack_Sizes in yacl_types.h
	OPT_DATA_SIZE   = 32,
	OPT_REG_SIZE    = 32,
};

//      TYPES

typedef enum Yacl_Error {
    YACL_SUCCESS        =  0,
    YACL_UNKNOWN_CMD    =  1,
    YACL_NO_CMD         =  2,
    YACL_USR_INPUT_FULL =  3,
    YACL_NO_CALLBACK    =  4,
    YACL_TOO_MANY_ARGS  =  5,
    YACL_ARG_TOO_LARGE  =  6,
    YACL_ARG_TOO_LONG   =  7,
    YACL_PARSE_BAD      =  8

} yacl_error_e;

typedef int (* usr_printf_f)(const char* format, ...);
typedef int (* usr_snprintf_f)(char* buffer, size_t count, const char* format, ...);

typedef struct Yacl_User_Print_Functions {
    usr_printf_f usr_printf;
    usr_snprintf_f usr_snprintf;

} yacl_usr_print_funcs_s;

typedef struct Yacl_Inout_Data {
    uint32_t data[OPT_DATA_SIZE];
	uint32_t read_data[OPT_DATA_SIZE];
    uint32_t reg[OPT_REG_SIZE];

    uint32_t addr;
    uint32_t state;

} yacl_inout_data_s;

typedef void (* yacl_cb_funcs_f)(yacl_inout_data_s* inout_data);

typedef struct Yacl_User_CallBacks {
    yacl_usr_print_funcs_s usr_print_funcs;

    yacl_cb_funcs_f usr_gpio_read;
    yacl_cb_funcs_f usr_gpio_write;
    yacl_cb_funcs_f usr_gpio_plot;

    yacl_cb_funcs_f usr_i2c_read;
    yacl_cb_funcs_f usr_i2c_write;
    yacl_cb_funcs_f usr_i2c_plot;

    yacl_cb_funcs_f usr_spi_read;
    yacl_cb_funcs_f usr_spi_write;
    yacl_cb_funcs_f usr_spi_plot;

} yacl_usr_callbacks_s;

typedef struct Yacl_Graph_Properties {
	// max number of samples in display
	// measurement steps
    uint8_t num_samples;
	uint16_t num_steps;
	
    // vertical measurement unit
    char units[GRAPH_UNITS_MAX_LEN + 1];

    // measurement range
    float upper_range;
    float lower_range;

	// supposed maximum points plotted
	uint32_t display_count;
	
} yacl_graph_s;

//      FUNCTIONS

void yacl_set_cb_null(yacl_usr_callbacks_s* usr_callbacks);
void yacl_init(yacl_usr_callbacks_s *usr_callbacks, yacl_graph_s *usr_graph);

void yacl_wr_buf(char data);
yacl_error_e yacl_parse_cmd();

yacl_error_e yacl_plot(float data);

const char*  yacl_error_desc(yacl_error_e error);

#endif //_YACL_H_
