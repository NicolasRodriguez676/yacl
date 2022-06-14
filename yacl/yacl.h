#ifndef _YACL_H_
#define _YACL_H_

#include <stddef.h>
#include <stdint.h>

enum Inout_Buffer_size{

#ifdef YACL_USR_INOUT_BFR_LEN
	INOUT_BUFR_LEN = YACL_USR_INOUT_BFR_LEN
#else
	INOUT_BUFR_LEN = 16
#endif
};

//      TYPES

typedef enum yacl_error {
	YACL_SUCCESS        = 0,
	YACL_UNKNOWN_CMD    = 1,
	YACL_NO_CMD         = 2,
	YACL_BUF_FULL       = 3,
	YACL_BUFRS_EMPTD    = 4,

} yacl_error_t;

typedef int (* usr_printf_t)(const char* format, ...);
typedef int (* usr_snprintf_t)(char* buffer, size_t count, const char* format, ...);

typedef struct usr_print_funcs {
	usr_printf_t usr_printf;
	usr_snprintf_t usr_snprintf;

} usr_print_funcs_t;

typedef struct yacl_inout_data {
	uint32_t bufr[INOUT_BUFR_LEN];
	uint32_t data;
	uint32_t addr;
	uint32_t beg_reg;
	uint32_t end_reg;

} yacl_inout_data_t;

typedef void (* cb_funcs_t)(yacl_inout_data_t* inout_data);

typedef struct yacl_usr_callbacks {
	usr_print_funcs_t usr_print_funcs;

	cb_funcs_t usr_gpio_write;
	cb_funcs_t usr_gpio_read;
	cb_funcs_t usr_i2c_read;
	cb_funcs_t usr_i2c_write;

} yacl_usr_callbacks_t;

//      FUNCTIONS

void yacl_init(yacl_usr_callbacks_t* usr_callbacks);

yacl_error_t yacl_parse_cmd();
yacl_error_t yacl_wr_buf(char data);
void         yacl_set_cb_null(yacl_usr_callbacks_t* usr_callbacks);

const char*  yacl_error_desc(yacl_error_t error);

#endif //_YACL_H_
