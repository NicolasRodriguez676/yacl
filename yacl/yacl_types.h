#ifndef _YACL_TYPES_H_
#define _YACL_TYPES_H_


enum Control_Characters {
	DELIM_SPACE         = 32,
	DELIM_NEWLINE       = 10,
	CNTRL_BACKSPACE     = 8,
};

enum Buffer_Lengths {
	MAX_DATA_LEN     = 128,
	MAX_TOKEN_LEN    = 16,
	MAX_TOKENS       = 6,
	TOKENS_LEN_IDX   = 15
};

enum Protocol_Call_Back_Indexes {
	HELP_CB_IDX     = 0,
	GPIO_CB_IDX     = 1,
	I2C_CB_IDX      = 2,
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
	TOKEN_CNT_WRITE_MIN = 5
};

enum Lut_Array_Sizes {
	MAX_PROTOCOLS = 4,
	MAX_ACTIONS = 2,
	MAX_FUNC_CBS = 3
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
	char* protocols[MAX_PROTOCOLS];
	char* actions[MAX_ACTIONS];
	cb_funcs_t* funcs[MAX_FUNC_CBS];

	uint32_t num_protocols;
	uint32_t num_actions;

} protocol_lut_cb_t;

#endif //_YACL_TYPES_H_
