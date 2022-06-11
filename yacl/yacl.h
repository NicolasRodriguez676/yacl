#ifndef _YACL_H_
#define _YACL_H_

#include <stddef.h>
#include <stdint.h>

//      TYPES

typedef enum yacl_error {
	YACL_SUCCESS        = 0,
	YACL_UNKNOWN_CMD    = 1,
	YACL_NO_CMD         = 2,
	YACL_BUF_FULL       = 3,
	YACL_BUFRS_EMPTD    = 4,

} yacl_error_t;

enum Action_Call_Back_Index {
	READ_CB_IDX = 0,
	WRITE_CB_IDX = 1
};

typedef int (* usr_printf_t)(const char* format, ...);
typedef int (* usr_snprintf_t)(char* buffer, size_t count, const char* format, ...);

typedef struct usr_print_funcs {
	usr_printf_t usr_printf;
	usr_snprintf_t usr_snprintf;

} usr_print_funcs_t;

typedef void (* cb_funcs_t)(uint32_t* data, uint32_t data_size);

typedef struct yacl_usr_callbacks {
	usr_print_funcs_t usr_print_funcs;
	cb_funcs_t gpio_funcs[2];
	cb_funcs_t i2c_funcs[2];

} yacl_usr_callbacks_t;

//      FUNCTIONS

void yacl_init(yacl_usr_callbacks_t* usr_callbacks);

yacl_error_t yacl_parse_cmd();
yacl_error_t yacl_wr_buf(char data);

const char* yacl_error_desc(yacl_error_t error);

#endif //_YACL_H_
