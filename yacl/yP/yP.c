
#line 1 "state-machine/yP.rl"
#include <string.h>
//#include <stdio.h>

#include "yP.h"
#include "../yacl.h"

extern usr_printf_t yacl_printf;

#line 11 "build/yP.c"
static const short _yG_key_offsets[] = {
        0, 0, 5, 7, 8, 9, 10, 11,
        12, 13, 14, 17, 18, 19, 20, 23,
        24, 28, 29, 32, 33, 37, 38, 41,
        42, 46, 47, 50, 51, 55, 56, 59,
        62, 68, 74, 75, 78, 84, 87, 93,
        99, 100, 103, 109, 112, 118, 124, 125,
        128, 134, 137, 143, 149, 150, 153, 159,
        160, 161, 164, 165, 166, 169, 170, 171,
        172, 173, 174, 175, 176, 177, 178, 179,
        180, 181, 182, 183, 184, 185, 186, 186,
        192, 196, 202, 206, 208, 214, 218, 220,
        225, 227, 231, 239, 240, 249, 253, 261,
        267, 277, 280, 290, 295, 304, 310, 320,
        323, 333, 338, 347, 353, 363, 366, 376,
        381
};

static const char _yG_trans_keys[] = {
        99, 104, 112, 114, 119, 108, 111, 101,
        97, 114, 10, 110, 102, 32, 103, 105,
        115, 112, 105, 111, 10, 32, 45, 45,
        100, 114, 115, 119, 32, 72, 48, 57,
        45, 100, 114, 115, 119, 32, 72, 48,
        57, 45, 100, 114, 115, 119, 32, 72,
        48, 57, 45, 100, 114, 115, 119, 32,
        72, 48, 57, 72, 48, 57, 48, 57,
        65, 70, 97, 102, 48, 57, 65, 70,
        97, 102, 32, 72, 48, 57, 48, 57,
        65, 70, 97, 102, 72, 48, 57, 48,
        57, 65, 70, 97, 102, 48, 57, 65,
        70, 97, 102, 32, 72, 48, 57, 48,
        57, 65, 70, 97, 102, 72, 48, 57,
        48, 57, 65, 70, 97, 102, 48, 57,
        65, 70, 97, 102, 32, 72, 48, 57,
        48, 57, 65, 70, 97, 102, 72, 48,
        57, 48, 57, 65, 70, 97, 102, 48,
        57, 65, 70, 97, 102, 32, 72, 48,
        57, 48, 57, 65, 70, 97, 102, 50,
        99, 10, 32, 45, 112, 105, 10, 32,
        45, 101, 108, 112, 10, 108, 111, 116,
        32, 101, 97, 100, 32, 114, 105, 116,
        101, 32, 10, 32, 44, 45, 48, 57,
        10, 32, 44, 45, 10, 32, 44, 45,
        48, 57, 10, 32, 44, 45, 32, 45,
        10, 32, 44, 45, 48, 57, 10, 32,
        44, 45, 32, 45, 10, 32, 44, 48,
        57, 10, 44, 10, 44, 48, 57, 10,
        44, 48, 57, 65, 70, 97, 102, 10,
        10, 32, 44, 48, 57, 65, 70, 97,
        102, 10, 32, 48, 57, 10, 32, 48,
        57, 65, 70, 97, 102, 10, 32, 44,
        45, 48, 57, 10, 32, 44, 45, 48,
        57, 65, 70, 97, 102, 10, 32, 45,
        10, 32, 44, 45, 48, 57, 65, 70,
        97, 102, 10, 32, 45, 48, 57, 10,
        32, 45, 48, 57, 65, 70, 97, 102,
        10, 32, 44, 45, 48, 57, 10, 32,
        44, 45, 48, 57, 65, 70, 97, 102,
        10, 32, 45, 10, 32, 44, 45, 48,
        57, 65, 70, 97, 102, 10, 32, 45,
        48, 57, 10, 32, 45, 48, 57, 65,
        70, 97, 102, 10, 32, 44, 45, 48,
        57, 10, 32, 44, 45, 48, 57, 65,
        70, 97, 102, 10, 32, 45, 10, 32,
        44, 45, 48, 57, 65, 70, 97, 102,
        10, 32, 45, 48, 57, 10, 32, 45,
        48, 57, 65, 70, 97, 102, 0
};

static const char _yG_single_lengths[] = {
        0, 5, 2, 1, 1, 1, 1, 1,
        1, 1, 3, 1, 1, 1, 3, 1,
        4, 1, 1, 1, 4, 1, 1, 1,
        4, 1, 1, 1, 4, 1, 1, 1,
        0, 0, 1, 1, 0, 1, 0, 0,
        1, 1, 0, 1, 0, 0, 1, 1,
        0, 1, 0, 0, 1, 1, 0, 1,
        1, 3, 1, 1, 3, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 0, 4,
        4, 4, 4, 2, 4, 4, 2, 3,
        2, 2, 2, 1, 3, 2, 2, 4,
        4, 3, 4, 3, 3, 4, 4, 3,
        4, 3, 3, 4, 4, 3, 4, 3,
        3
};

static const char _yG_range_lengths[] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1, 0, 0, 0, 1, 0,
        0, 0, 1, 0, 0, 0, 1, 1,
        3, 3, 0, 1, 3, 1, 3, 3,
        0, 1, 3, 1, 3, 3, 0, 1,
        3, 1, 3, 3, 0, 1, 3, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1,
        0, 1, 0, 0, 1, 0, 0, 1,
        0, 1, 3, 0, 3, 1, 3, 1,
        3, 0, 3, 1, 3, 1, 3, 0,
        3, 1, 3, 1, 3, 0, 3, 1,
        3
};

static const short _yG_index_offsets[] = {
        0, 0, 6, 9, 11, 13, 15, 17,
        19, 21, 23, 27, 29, 31, 33, 37,
        39, 44, 46, 49, 51, 56, 58, 61,
        63, 68, 70, 73, 75, 80, 82, 85,
        88, 92, 96, 98, 101, 105, 108, 112,
        116, 118, 121, 125, 128, 132, 136, 138,
        141, 145, 148, 152, 156, 158, 161, 165,
        167, 169, 173, 175, 177, 181, 183, 185,
        187, 189, 191, 193, 195, 197, 199, 201,
        203, 205, 207, 209, 211, 213, 215, 216,
        222, 227, 233, 238, 241, 247, 252, 255,
        260, 263, 267, 273, 275, 282, 286, 292,
        298, 306, 310, 318, 323, 330, 336, 344,
        348, 356, 361, 368, 374, 382, 386, 394,
        399
};

static const unsigned char _yG_indicies[] = {
        1, 2, 3, 4, 5, 0, 6, 7,
        0, 8, 0, 9, 0, 10, 0, 11,
        0, 12, 0, 13, 0, 14, 0, 15,
        16, 17, 0, 18, 0, 19, 0, 20,
        0, 11, 21, 22, 0, 22, 0, 23,
        24, 25, 26, 0, 27, 0, 29, 28,
        0, 30, 0, 31, 32, 33, 34, 0,
        35, 0, 37, 36, 0, 38, 0, 39,
        40, 41, 42, 0, 43, 0, 45, 44,
        0, 46, 0, 47, 48, 49, 50, 0,
        51, 0, 53, 52, 0, 55, 54, 0,
        56, 56, 56, 0, 57, 57, 57, 0,
        58, 0, 60, 59, 0, 61, 61, 61,
        0, 63, 62, 0, 64, 64, 64, 0,
        65, 65, 65, 0, 66, 0, 68, 67,
        0, 69, 69, 69, 0, 71, 70, 0,
        72, 72, 72, 0, 73, 73, 73, 0,
        74, 0, 76, 75, 0, 77, 77, 77,
        0, 79, 78, 0, 80, 80, 80, 0,
        81, 81, 81, 0, 82, 0, 84, 83,
        0, 85, 85, 85, 0, 86, 0, 87,
        0, 11, 21, 22, 0, 88, 0, 89,
        0, 11, 21, 22, 0, 90, 0, 91,
        0, 92, 0, 11, 0, 93, 0, 94,
        0, 95, 0, 14, 0, 96, 0, 97,
        0, 98, 0, 14, 0, 99, 0, 100,
        0, 101, 0, 102, 0, 14, 0, 0,
        103, 104, 105, 106, 107, 0, 11, 108,
        109, 30, 0, 110, 111, 112, 113, 114,
        0, 115, 116, 117, 38, 0, 116, 38,
        0, 118, 119, 120, 121, 122, 0, 123,
        124, 125, 46, 0, 124, 46, 0, 126,
        127, 128, 129, 0, 11, 130, 0, 126,
        128, 131, 0, 126, 128, 132, 132, 132,
        0, 11, 0, 126, 127, 128, 133, 133,
        133, 0, 127, 127, 134, 0, 127, 127,
        135, 135, 135, 0, 118, 136, 120, 121,
        137, 0, 118, 136, 120, 121, 138, 138,
        138, 0, 123, 124, 46, 0, 118, 119,
        120, 121, 139, 139, 139, 0, 119, 119,
        121, 140, 0, 119, 119, 121, 141, 141,
        141, 0, 110, 142, 112, 113, 143, 0,
        110, 142, 112, 113, 144, 144, 144, 0,
        115, 116, 38, 0, 110, 111, 112, 113,
        145, 145, 145, 0, 111, 111, 113, 146,
        0, 111, 111, 113, 147, 147, 147, 0,
        103, 148, 105, 106, 149, 0, 103, 148,
        105, 106, 150, 150, 150, 0, 11, 108,
        30, 0, 103, 104, 105, 106, 151, 151,
        151, 0, 104, 104, 106, 152, 0, 104,
        104, 106, 153, 153, 153, 0, 0
};

static const char _yG_trans_targs[] = {
        0, 2, 61, 65, 69, 73, 3, 7,
        4, 5, 6, 78, 8, 9, 10, 11,
        55, 58, 12, 13, 14, 15, 16, 17,
        17, 52, 52, 18, 79, 51, 20, 21,
        21, 46, 46, 22, 81, 45, 24, 25,
        25, 40, 40, 26, 84, 39, 28, 29,
        29, 34, 34, 30, 87, 33, 89, 32,
        90, 92, 35, 93, 36, 94, 95, 38,
        96, 98, 41, 99, 42, 100, 101, 44,
        102, 104, 47, 105, 48, 106, 107, 50,
        108, 110, 53, 111, 54, 112, 56, 57,
        59, 60, 62, 63, 64, 66, 67, 68,
        70, 71, 72, 74, 75, 76, 77, 80,
        109, 49, 20, 79, 19, 49, 82, 103,
        43, 24, 81, 83, 23, 43, 85, 97,
        37, 28, 84, 86, 27, 37, 88, 91,
        31, 87, 31, 89, 90, 92, 93, 94,
        27, 95, 96, 98, 99, 100, 23, 101,
        102, 104, 105, 106, 19, 107, 108, 110,
        111, 112
};

static const char _yG_trans_actions[] = {
        1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 5,
        6, 7, 8, 0, 9, 0, 0, 5,
        6, 7, 8, 0, 9, 0, 0, 5,
        6, 7, 8, 0, 9, 0, 0, 5,
        6, 7, 8, 0, 9, 0, 9, 0,
        9, 9, 0, 9, 0, 9, 9, 0,
        9, 9, 0, 9, 0, 9, 9, 0,
        9, 9, 0, 9, 0, 9, 9, 0,
        9, 9, 0, 9, 0, 9, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 17,
        17, 17, 17, 0, 0, 0, 17, 17,
        17, 17, 0, 0, 0, 0, 17, 17,
        17, 17, 0, 0, 0, 0, 17, 17,
        17, 0, 0, 0, 0, 0, 0, 0,
        17, 0, 0, 0, 0, 0, 17, 0,
        0, 0, 0, 0, 17, 0, 0, 0,
        0, 0
};

static const char _yG_to_state_actions[] = {
        0, 0, 0, 0, 0, 0, 2, 0,
        0, 3, 0, 0, 0, 0, 4, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 11, 0, 0, 12, 0, 0, 0,
        13, 0, 0, 0, 14, 0, 0, 0,
        15, 0, 0, 0, 0, 16, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0
};

static const char _yG_from_state_actions[] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        10, 10, 0, 0, 10, 0, 10, 10,
        0, 0, 10, 0, 10, 10, 0, 0,
        10, 0, 10, 10, 0, 0, 10, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0
};

static const char _yG_eof_actions[] = {
        0, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 0, 17,
        0, 17, 0, 0, 17, 0, 0, 17,
        0, 17, 17, 0, 17, 17, 17, 17,
        17, 0, 17, 17, 17, 17, 17, 0,
        17, 17, 17, 17, 17, 0, 17, 17,
        17
};

static const int yG_start = 1;
static const int yG_first_final = 78;
static const int yG_error = 0;

static const int yG_en_main = 1;


#line 150 "state-machine/yP.rl"


void yaclG_init()
{

}

bool yaclG_parse(char* p, char* pe, walk_stack_s* yG_stack)
{
    int cs = 0;

#line 309 "build/yP.c"
    {
        cs = yG_start;
    }

#line 161 "state-machine/yP.rl"

    yG_stack->bufr_begin = p;

    option_data_stack_s* option;

    char* eof = pe;


#line 323 "build/yP.c"
    {
        int _klen;
        const char *_keys;
        int _trans;

        if ( p == pe )
            goto _test_eof;
        if ( cs == 0 )
            goto _out;
        _resume:
        switch ( _yG_from_state_actions[cs] ) {
            case 10:
#line 78 "state-machine/yP.rl"
            {
                option->base[option->idx] = 16;
            }
                break;
#line 341 "build/yP.c"
        }

        _keys = _yG_trans_keys + _yG_key_offsets[cs];
        _trans = _yG_index_offsets[cs];

        _klen = _yG_single_lengths[cs];
        if ( _klen > 0 ) {
            const char *_lower = _keys;
            const char *_mid;
            const char *_upper = _keys + _klen - 1;
            while (1) {
                if ( _upper < _lower )
                    break;

                _mid = _lower + ((_upper-_lower) >> 1);
                if ( (*p) < *_mid )
                    _upper = _mid - 1;
                else if ( (*p) > *_mid )
                    _lower = _mid + 1;
                else {
                    _trans += (unsigned int)(_mid - _keys);
                    goto _match;
                }
            }
            _keys += _klen;
            _trans += _klen;
        }

        _klen = _yG_range_lengths[cs];
        if ( _klen > 0 ) {
            const char *_lower = _keys;
            const char *_mid;
            const char *_upper = _keys + (_klen<<1) - 2;
            while (1) {
                if ( _upper < _lower )
                    break;

                _mid = _lower + (((_upper-_lower) >> 1) & ~1);
                if ( (*p) < _mid[0] )
                    _upper = _mid - 2;
                else if ( (*p) > _mid[1] )
                    _lower = _mid + 2;
                else {
                    _trans += (unsigned int)((_mid - _keys)>>1);
                    goto _match;
                }
            }
            _trans += _klen;
        }

        _match:
        _trans = _yG_indicies[_trans];
        cs = _yG_trans_targs[_trans];

        if ( _yG_trans_actions[_trans] == 0 )
            goto _again;

        switch ( _yG_trans_actions[_trans] ) {
            case 5:
#line 29 "state-machine/yP.rl"
            {
                yG_stack->valid_options[OPT_DATA] = true;
                option = &yG_stack->options[OPT_DATA];
            }
                break;
            case 6:
#line 34 "state-machine/yP.rl"
            {
                yG_stack->valid_options[OPT_REG] = true;
                option = &yG_stack->options[OPT_REG];
            }
                break;
            case 7:
#line 39 "state-machine/yP.rl"
            {
                yG_stack->valid_options[OPT_ADDR] = true;
                option = &yG_stack->options[OPT_ADDR];
            }
                break;
            case 8:
#line 44 "state-machine/yP.rl"
            {
                yG_stack->valid_options[OPT_STATE] = true;
                option = &yG_stack->options[OPT_STATE];
            }
                break;
            case 9:
#line 49 "state-machine/yP.rl"
            {
                if (option->idx >= 32)
                {
                    uint32_t bufr_idx = p - yG_stack->bufr_begin;
                    yacl_printf("parse error at index %u. too many arguments\n", bufr_idx);
                    return false;
                }

                option->args[option->idx] = p;
            }
                break;
            case 17:
#line 60 "state-machine/yP.rl"
            {
                (*p) = '\0';

                uint32_t max_str_len = 10;

                if (option->base[option->idx] == 16)
                    max_str_len = 8;

                if (strlen(option->args[option->idx]) > max_str_len)
                {
                    uint32_t bufr_idx = p - yG_stack->bufr_begin;
                    yacl_printf("parse error at index %u. argument too large\n", bufr_idx);
                    return false;
                }

                ++option->idx;
            }
                break;
            case 1:
#line 84 "state-machine/yP.rl"
            {
                uint32_t bufr_idx = p - yG_stack->bufr_begin;
                yacl_printf("parse error at index %u. misspelled or unknown argument\n", bufr_idx);
                return false;
            }
                break;
#line 469 "build/yP.c"
        }

        _again:
        switch ( _yG_to_state_actions[cs] ) {
            case 16:
#line 14 "state-machine/yP.rl"
            { yG_stack->action = ACTION_WRITE;}
                break;
            case 15:
#line 15 "state-machine/yP.rl"
            { yG_stack->action = ACTION_READ; }
                break;
            case 14:
#line 16 "state-machine/yP.rl"
            { yG_stack->action = ACTION_PLOT; }
                break;
            case 3:
#line 17 "state-machine/yP.rl"
            { yG_stack->action = ACTION_CONF; }
                break;
            case 13:
#line 18 "state-machine/yP.rl"
            { yG_stack->action = ACTION_HELP; }
                break;
            case 2:
#line 19 "state-machine/yP.rl"
            { yG_stack->action = ACTION_CLEAR;}
                break;
            case 4:
#line 23 "state-machine/yP.rl"
            { yG_stack->stream = STREAM_GPIO; }
                break;
            case 11:
#line 24 "state-machine/yP.rl"
            { yG_stack->stream = STREAM_I2C;  }
                break;
            case 12:
#line 25 "state-machine/yP.rl"
            { yG_stack->stream = STREAM_SPI;  }
                break;
#line 510 "build/yP.c"
        }

        if ( cs == 0 )
            goto _out;
        if ( ++p != pe )
            goto _resume;
        _test_eof: {}
        if ( p == eof )
        {
            switch ( _yG_eof_actions[cs] ) {
                case 17:
#line 60 "state-machine/yP.rl"
                {
                    (*p) = '\0';

                    uint32_t max_str_len = 10;

                    if (option->base[option->idx] == 16)
                        max_str_len = 8;

                    if (strlen(option->args[option->idx]) > max_str_len)
                    {
                        uint32_t bufr_idx = p - yG_stack->bufr_begin;
                        yacl_printf("parse error at index %u. argument too large\n", bufr_idx);
                        return false;
                    }

                    ++option->idx;
                }
                    break;
                case 1:
#line 84 "state-machine/yP.rl"
                {
                    uint32_t bufr_idx = p - yG_stack->bufr_begin;
                    yacl_printf("parse error at index %u. misspelled or unknown argument\n", bufr_idx);
                    return false;
                }
                    break;
#line 549 "build/yP.c"
            }
        }

        _out: {}
    }

#line 169 "state-machine/yP.rl"

    return true;
}