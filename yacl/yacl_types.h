#ifndef _YACL_TYPES_H_
#define _YACL_TYPES_H_


enum Misc {
	NO_CB   = -1,
};

enum Control_Characters {
	DELIM_SPACE         = 32,
	DELIM_NEWLINE       = 10,
	CNTRL_BACKSPACE     = 8,
};

enum Buffer_Lengths {
	DATA_LEN_MAX     = 128,
	MAX_TOKEN_LEN    = 16,
	MAX_TOKENS       = 12,
	TOKENS_LEN_IDX   = 15
};

enum Protocol_Call_Back_Index {
	GPIO_CB_IDX = 0,
	I2C_CB_IDX = 1,
};

enum Lut_Array_Sizes {
	MAX_PROTOCOLS = 4,
	MAX_ACTIONS = 2
};

typedef struct error_desc {
	const yacl_error_t code;
	const char* msg;

} error_desc_t;

typedef struct ring_buffer {
	uint8_t bufr[DATA_LEN_MAX];
	uint32_t head;
	uint32_t tail;

} ring_buffer_t;

typedef struct data_buffer {
	uint8_t bufr[MAX_TOKEN_LEN * MAX_TOKENS];
	uint32_t idx;

	uint8_t* tok_array[MAX_TOKENS];
	uint32_t tok_beg_idx;
	uint32_t tok_idx;

} data_buffer_t;

typedef struct comm_lut_cb {
	char* protocols[MAX_PROTOCOLS];
	char* actions[MAX_ACTIONS];
	cb_funcs_t* funcs[2];

	uint32_t num_protocols;
	uint32_t num_actions;

} comm_lut_cb_t;

#endif //_YACL_TYPES_H_
