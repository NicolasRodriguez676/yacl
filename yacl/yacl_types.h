#ifndef _YACL_TYPES_H_
#define _YACL_TYPES_H_

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
    STREAM_NONE = 0,
    STREAM_GPIO = 0,
    STREAM_I2C  = 1,
    STREAM_SPI  = 2,

    NUM_STREAMS  = 3

} stream_e;

enum Option_Stack_Sizes {
						  // yacl_inout_data_s
    OPT_DATA_SIZE   = 32, // data
    OPT_REG_SIZE    = 32, // reg
    OPT_ADDR_SIZE   = 1,
    OPT_STATE_SIZE  = 1
};

enum Option_Stack_Offsets {
    OPT_DATA_OFFSET   = 0,
    OPT_REG_OFFSET    = OPT_DATA_SIZE,
    OPT_ADDR_OFFSET   = OPT_DATA_SIZE + OPT_REG_SIZE,
    OPT_STATE_OFFSET  = OPT_DATA_SIZE + OPT_REG_SIZE + OPT_STATE_SIZE,
};

typedef enum Option_Stack_Indexes {
    OPT_DATA      = 0,
    OPT_REG       = 1,
    OPT_ADDR      = 2,
    OPT_STATE     = 3,

	OPT_NONE      = 0,
    NUM_OPTIONS   = 4
} opt_stack_idx_e;

typedef struct Option_Data_Stack {
    char** args;
    uint32_t* base;

    uint32_t idx;

} option_data_stack_s;

typedef struct Walk_Stack {
	uint32_t bufr_begin;

    action_e action;
    stream_e stream;

    // options
    bool valid_options[4];
    option_data_stack_s options[4];

} walk_stack_s;

enum Control_Characters {
    DELIM_SPACE         = 32,
    DELIM_NEWLINE       = 10,
    TERM_ESC            = 27,
    TERM_CSI            = '[',
    TERM_CSR_LEFT       = 'D',
    TERM_CSR_RIGHT      = 'C',
	TERM_SEQ_LEN        = 3,
	TERM_BACKSPACE      = 127,
    EXIT_PLOT           = 'q'
};

enum Buffer_Attributes {
    USER_INPUT_LEN   = 256,
    INPUT_DATA_LEN   = 32,
    INPUT_BUFR_MASK  = 0x1f
};

typedef struct error_desc {
    const yacl_error_e code;
    const char* msg;

} error_desc_t;

typedef struct ring_buffer {
    uint8_t bufr[INPUT_DATA_LEN];
    uint8_t head;
    uint8_t tail;
    bool overrun;

} ring_buffer_s;

typedef struct user_input_string {
	uint8_t bufr[USER_INPUT_LEN];
	uint8_t index;
	uint32_t cursor;

} user_input_string_s;

typedef struct callback_lut {
    cb_funcs_f funcs[NUM_USR_DEF_ACTIONS][NUM_STREAMS];

} cb_lut_s;

#endif //_YACL_TYPES_H_
