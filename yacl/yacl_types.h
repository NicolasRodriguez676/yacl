#ifndef _YACL_TYPES_H_
#define _YACL_TYPES_H_


enum Control_Characters {
    DELIM_SPACE         = 32,
    DELIM_NEWLINE       = 10,
    CNTRL_BACKSPACE     = 127,
};

enum Buffer_Lengths {
    MAX_DATA_LEN     = 128,
    MAX_TOKEN_LEN    = 16,
    MAX_TOKENS       = 6,
    TOKENS_LEN_IDX   = 15
};

enum Action_Call_Back_Index {
    READ_CB_IDX  = 0,
    WRITE_CB_IDX = 1,
    PLOT_CB_IDX = 2
};

enum Protocol_Call_Back_Indexes {
    GPIO_CB_IDX     = 0,
    I2C_CB_IDX      = 1,
    SPI_CB_IDX      = 2,
    HELP_CB_IDX     = 3,
};

enum Token_Arg_Indexes {
    ARG_ADDR_IDX        = 2,
    ARG_BEG_REG_IDX     = 3,
    ARG_END_REG_IDX     = 4,
    ARG_DATA0_IDX       = 4,
    ARG_DATA1_IDX       = 5,

    ARG_PROTOCOL_IDX    = 0,
    ARG_ACTION_IDX      = 1
};

enum Token_Count_Checks {
    ARG_READ_RANGE_CNT  = 4,
    ARG_WRITE_RANGE_CNT = 5,

    TOKEN_CNT_READ_MIN  = 4,
    TOKEN_CNT_WRITE_MIN = 5,

    TOKEN_CNT_PLOT_MIN  = 4
};

enum Lut_Array_Sizes {
    NUM_PROTOCOLS = 4,
    NUM_ACTIONS   = 3,
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

typedef struct protocol_lut_cb {
    char* protocols[NUM_PROTOCOLS + 1];
    char* actions[NUM_ACTIONS];
    cb_funcs_t funcs[(NUM_PROTOCOLS * NUM_ACTIONS) + 1][NUM_ACTIONS];

    bool not_null_cbs[NUM_PROTOCOLS];
    uint32_t num_not_null_cbs;

} protocol_lut_cb_t;

#endif //_YACL_TYPES_H_
