#ifndef _YACL_TYPES_H_
#define _YACL_TYPES_H_

#include <stdbool.h>

#include "yacl.h"

typedef enum Yacl_Actions {
    ACTION_WRITE = 0,
    ACTION_READ  = 1,
    ACTION_PLOT  = 2,
    ACTION_SET   = 3,
    ACTION_HELP  = 4,
    ACTION_CLEAR = 5,

    ACTION_NONE  = 0,
    NUM_USR_DEF_ACTIONS  = 3

} action_e;

typedef enum Yacl_Streams {
    STREAM_GPIO = 0,
    STREAM_I2C  = 1,
    STREAM_SPI  = 2,

	STREAM_NONE = 0,
    NUM_STREAMS  = 3

} stream_e;

enum Option_Stack_Sizes {
//    OPT_DATA_SIZE   = 32,
//    OPT_REG_SIZE    = 32,
	OPT_ADDR_SIZE   = 1,
    OPT_STATE_SIZE  = 1,
    OPT_UPPER_SIZE  = 1,
    OPT_LOWER_SIZE  = 1,
    OPT_NSMPL_SIZE  = 1,
    OPT_NSTPS_SIZE  = 1,
    OPT_UNITS_SIZE  = 1,
	
	OPT_STACK_SIZE = OPT_DATA_SIZE + OPT_REG_SIZE + OPT_ADDR_SIZE + OPT_STATE_SIZE + OPT_UPPER_SIZE + OPT_LOWER_SIZE + OPT_NSMPL_SIZE + OPT_NSTPS_SIZE + OPT_UNITS_SIZE
};

enum Option_Stack_Offsets {
    OPT_DATA_OFFSET   = 0,
    OPT_REG_OFFSET    = OPT_DATA_SIZE,
    OPT_ADDR_OFFSET   = OPT_DATA_SIZE + OPT_REG_SIZE,
    OPT_STATE_OFFSET  = OPT_DATA_SIZE + OPT_REG_SIZE + OPT_ADDR_SIZE,
    OPT_UPPER_OFFSET  = OPT_DATA_SIZE + OPT_REG_SIZE + OPT_ADDR_SIZE + OPT_STATE_SIZE,
    OPT_LOWER_OFFSET  = OPT_DATA_SIZE + OPT_REG_SIZE + OPT_ADDR_SIZE + OPT_STATE_SIZE + OPT_UPPER_SIZE,
    OPT_NSMPL_OFFSET  = OPT_DATA_SIZE + OPT_REG_SIZE + OPT_ADDR_SIZE + OPT_STATE_SIZE + OPT_UPPER_SIZE + OPT_LOWER_SIZE,
    OPT_NSTPS_OFFSET  = OPT_DATA_SIZE + OPT_REG_SIZE + OPT_ADDR_SIZE + OPT_STATE_SIZE + OPT_UPPER_SIZE + OPT_LOWER_SIZE + OPT_NSMPL_SIZE,
    OPT_UNITS_OFFSET  = OPT_DATA_SIZE + OPT_REG_SIZE + OPT_ADDR_SIZE + OPT_STATE_SIZE + OPT_UPPER_SIZE + OPT_LOWER_SIZE + OPT_NSMPL_SIZE + OPT_NSTPS_SIZE
};

typedef enum Option_Stack_Indexes {
    OPT_DATA        = 0,
    OPT_REG         = 1,
    OPT_ADDR        = 2,
    OPT_STATE       = 3,
    OPT_UPPER_BOUND = 4,
    OPT_LOWER_BOUND = 5,
    OPT_NUM_SAMPLES = 6,
    OPT_NUM_STEPS   = 7,
    OPT_UNITS       = 8,

	OPT_NONE        = 0,
    NUM_OPTIONS     = 9
} option_stack_index_e;

typedef struct Option_Data_Stack {
    char** args;
    uint32_t* base;

    uint32_t idx;

} option_data_stack_s;

typedef struct Walk_Stack {
	char* bufr_begin;
	uint32_t bufr_err_beg;
	uint32_t bufr_err_idx;
	
    action_e action;
    stream_e stream;

    // options
    bool valid_options[NUM_OPTIONS];
    option_data_stack_s options[NUM_OPTIONS];

} walk_stack_s;

enum Control_Characters {
    ASCII_SP            = 32,
    ASCII_CR            = 13,
    TERM_ESC            = 27,
    TERM_CSI            = '[',
    TERM_CSR_LEFT       = 'D',
    TERM_CSR_RIGHT      = 'C',
	TERM_BACKSPACE      = 127,
    EXIT_PLOT           = 'q'
};

enum Buffer_Attributes {
    USER_INPUT_LEN   = 256,
    INPUT_DATA_LEN   = 32,
    INPUT_BUFR_MASK  = 0x1f
};

typedef struct Error_Description {
    const yacl_error_e code;
    const char* msg;

} error_desc_t;

typedef struct Ring_Buffer {
    uint8_t bufr[INPUT_DATA_LEN];
    uint8_t head;
    uint8_t tail;
    bool overrun;

} ring_buffer_s;

typedef struct User_Input_String {
	uint8_t bufr[USER_INPUT_LEN];
	uint8_t index;
	uint8_t cursor;

} user_input_string_s;

typedef struct CallBack_LUT {
    yacl_cb_funcs_f funcs[NUM_USR_DEF_ACTIONS][NUM_STREAMS];

} cb_lut_s;

#endif //_YACL_TYPES_H_
