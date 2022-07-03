#include <stdint.h>
#include <stdbool.h>

//        TYPES

typedef enum Yacl_Action_Flags {

    ACTION_NONE  = 0,
    ACTION_WRITE = 1,
    ACTION_READ  = 2,
    ACTION_PLOT  = 3,
    ACTION_CONF  = 4,
    ACTION_HELP  = 5,
    ACTION_CLEAR = 6
    
} act_flag_e;

typedef enum Yacl_Stream_Flags {

    STREAM_NONE = 0,
    STREAM_GPIO = 1,
    STREAM_I2C  = 2,
    STREAM_SPI  = 3

} str_flag_e;

typedef enum Option_Stack_Indexes {

    OPT_DATA      = 0,
    OPT_REG       = 1,
    OPT_ADDR      = 2,
    OPT_STATE     = 3

} option_stack_idx_e;

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

//        FUNCTION PROTOTYPES

void yaclG_init();
bool yaclG_parse(char* p, char* pe, walk_stack_s* yG_stack);