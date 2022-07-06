#ifndef _YACL_H_
#define _YACL_H_

#include <stddef.h>
#include <stdint.h>

// for extracting more than default number of consecutive register data,
// define this macro using CMake
enum Inout_Buffer_size {
#ifdef YACL_USR_INOUT_BFR_LEN
    INOUT_BUFR_LEN = YACL_USR_INOUT_BFR_LEN
#else
    INOUT_BUFR_LEN = 16
#endif
};

//      TYPES

typedef enum yacl_error {
    YACL_SUCCESS        =  0,
    YACL_UNKNOWN_CMD    =  1,
    YACL_NO_CMD         =  2,
    YACL_BUF_FULL       =  3,
    YACL_BUFRS_EMPTD    =  4,
    YACL_ARG_OVRN       =  5,
    YACL_TOO_MANY_ARGS  =  6,
    YACL_NOT_ENUF_ARGS  =  7,
    YACL_INVALID_ARG    =  8,
    YACL_INOUT_BUFR     =  9,
    YACL_NO_CALLBACK    = 10,
    YACL_PARSE_BAD      = 11,

} yacl_error_e;

typedef int (* usr_printf_f)(const char* format, ...);
typedef int (* usr_snprintf_f)(char* buffer, size_t count, const char* format, ...);

typedef struct usr_print_funcs {
    usr_printf_f usr_printf;
    usr_snprintf_f usr_snprintf;

} usr_print_funcs_s;

typedef struct yacl_inout_data {
    uint32_t data[32];
	uint32_t read_data[32];
    uint32_t reg[32];

    uint32_t addr;
    uint32_t state;

} yacl_inout_data_s;

typedef void (* cb_funcs_f)(yacl_inout_data_s* inout_data);

typedef struct yacl_usr_callbacks {
    usr_print_funcs_s usr_print_funcs;

    cb_funcs_f usr_gpio_read;
    cb_funcs_f usr_gpio_write;
    cb_funcs_f usr_gpio_plot;

    cb_funcs_f usr_i2c_read;
    cb_funcs_f usr_i2c_write;
    cb_funcs_f usr_i2c_plot;

    cb_funcs_f usr_spi_read;
    cb_funcs_f usr_spi_write;
    cb_funcs_f usr_spi_plot;

} yacl_usr_callbacks_s;

typedef struct yacl_graph_properties {
    // max number of samples in display  -- uint8_t
    uint8_t num_samples;

    // vertical measurement unit         -- char*
    char* units;

    // measurement range                 -- float, float
    float upper_range;
    float lower_range;

    // measurement steps                 -- uint32_t
    uint32_t num_steps;

} yacl_graph_s;

//      FUNCTIONS

void yacl_init(yacl_usr_callbacks_s *usr_callbacks, yacl_graph_s *usr_graph);

yacl_error_e yacl_parse_cmd();
void         yacl_wr_buf(char data);
yacl_error_e yacl_plot(float data);

void         yacl_set_cb_null(yacl_usr_callbacks_s* usr_callbacks);

const char*  yacl_error_desc(yacl_error_e error);

#endif //_YACL_H_
