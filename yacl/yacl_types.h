#ifndef _YACL_TYPES_H_
#define _YACL_TYPES_H_

#include "yacl.h"

typedef enum Yacl_Actions {
    ACTION_WRITE = 0,
    ACTION_READ  = 1,
    ACTION_PLOT  = 2,
    ACTION_CONF  = 3,
    ACTION_HELP  = 4,
    ACTION_CLEAR = 5,

    ACTION_NONE  = 0,
    NUM_USR_DEF_ACTIONS  = 3

} act_flag_e;

typedef enum Yacl_Streams {
    STREAM_NONE = 0,
    STREAM_GPIO = 0,
    STREAM_I2C  = 1,
    STREAM_SPI  = 2,

    NUM_STREAMS  = 3

} str_flag_e;

enum Option_Stack_Sizes {
    OPT_DATA_SIZE   = 32,
    OPT_REG_SIZE    = 32,
    OPT_ADDR_SIZE   = 1,
    OPT_STATE_SIZE  = 1,
};

enum Option_Stack_Offsets {
    OPT_DATA_OFFSET   = 0,
    OPT_REG_OFFSET    = OPT_DATA_SIZE,
    OPT_ADDR_OFFSET   = OPT_DATA_SIZE + OPT_REG_SIZE,
    OPT_STATE_OFFSET  = OPT_DATA_SIZE + OPT_REG_SIZE + OPT_STATE_SIZE,
};

enum Option_Stack_Indexes {
    OPT_DATA      = 0,
    OPT_REG       = 1,
    OPT_ADDR      = 2,
    OPT_STATE     = 3,

    NUM_OPTIONS   = 4
};

typedef struct Option_Data_Stack {
    char** args;
    uint32_t* base;

    uint32_t idx;

} option_data_stack_s;

typedef struct Walk_Stack {
    char* bufr_begin;

    // actions
    act_flag_e action;

    // streams
    str_flag_e stream;

    // options
    bool valid_options[4];
    option_data_stack_s options[4];

} walk_stack_s;

enum Control_Characters {
    DELIM_SPACE         = 32,
    DELIM_NEWLINE       = 10,
    CNTRL_BACKSPACE     = 127,
    EXIT_PLOT           = 'q'
};

enum Buffer_Lengths {
    MAX_DATA_LEN     = 128,
    MAX_TOKEN_LEN    = 16,
    MAX_TOKENS       = 6,
    TOKENS_LEN_IDX   = 15
};

typedef struct error_desc {
    const yacl_error_t code;
    const char* msg;

} error_desc_t;

typedef struct ring_buffer {
    uint8_t bufr[MAX_DATA_LEN];
    uint32_t head;
    uint32_t tail;

} ring_buffer_t;

typedef struct data_buffer {
    uint8_t bufr[MAX_TOKEN_LEN * MAX_TOKENS];
    uint32_t idx;

    uint8_t* tok_array[MAX_TOKENS];
    uint32_t tok_cnt;

} data_buffer_t;

typedef struct callback_lut {
    cb_funcs_t funcs[NUM_USR_DEF_ACTIONS][NUM_STREAMS];

    bool not_null_cbs[NUM_USR_DEF_ACTIONS][NUM_STREAMS];
    uint32_t num_not_null_cbs;

} cb_lut_t;

#endif //_YACL_TYPES_H_
