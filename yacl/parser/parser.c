
#line 1 "yacl/ragel/parser.rl"
#include <string.h>

#include "parser.h"

extern usr_printf_t yacl_printf;


#line 11 "yacl/parser/parser.c"
static const short _yacl_parser_key_offsets[] = {
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
	192, 195, 201, 204, 210, 213, 218, 219, 
	223, 225, 233, 242, 246, 254, 256, 262, 
	266, 276, 286, 291, 300, 302, 308, 312, 
	322, 332, 337, 346, 352, 356, 366, 376, 
	381
};

static const char _yacl_parser_trans_keys[] = {
	99, 104, 112, 114, 119, 108, 111, 101, 
	97, 114, 46, 110, 102, 32, 103, 105, 
	115, 112, 105, 111, 32, 45, 46, 45, 
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
	99, 32, 45, 46, 112, 105, 32, 45, 
	46, 101, 108, 112, 46, 108, 111, 116, 
	32, 101, 97, 100, 32, 114, 105, 116, 
	101, 32, 32, 44, 45, 46, 48, 57, 
	32, 45, 46, 32, 44, 45, 46, 48, 
	57, 32, 45, 46, 32, 44, 45, 46, 
	48, 57, 32, 45, 46, 32, 44, 46, 
	48, 57, 46, 44, 46, 48, 57, 44, 
	46, 44, 46, 48, 57, 65, 70, 97, 
	102, 32, 44, 46, 48, 57, 65, 70, 
	97, 102, 32, 46, 48, 57, 32, 46, 
	48, 57, 65, 70, 97, 102, 32, 45, 
	32, 44, 45, 46, 48, 57, 32, 44, 
	45, 46, 32, 44, 45, 46, 48, 57, 
	65, 70, 97, 102, 32, 44, 45, 46, 
	48, 57, 65, 70, 97, 102, 32, 45, 
	46, 48, 57, 32, 45, 46, 48, 57, 
	65, 70, 97, 102, 32, 45, 32, 44, 
	45, 46, 48, 57, 32, 44, 45, 46, 
	32, 44, 45, 46, 48, 57, 65, 70, 
	97, 102, 32, 44, 45, 46, 48, 57, 
	65, 70, 97, 102, 32, 45, 46, 48, 
	57, 32, 45, 46, 48, 57, 65, 70, 
	97, 102, 32, 44, 45, 46, 48, 57, 
	32, 44, 45, 46, 32, 44, 45, 46, 
	48, 57, 65, 70, 97, 102, 32, 44, 
	45, 46, 48, 57, 65, 70, 97, 102, 
	32, 45, 46, 48, 57, 32, 45, 46, 
	48, 57, 65, 70, 97, 102, 0
};

static const char _yacl_parser_single_lengths[] = {
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
	3, 4, 3, 4, 3, 3, 1, 2, 
	2, 2, 3, 2, 2, 2, 4, 4, 
	4, 4, 3, 3, 2, 4, 4, 4, 
	4, 3, 3, 4, 4, 4, 4, 3, 
	3
};

static const char _yacl_parser_range_lengths[] = {
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
	0, 1, 0, 1, 0, 1, 0, 1, 
	0, 3, 3, 1, 3, 0, 1, 0, 
	3, 3, 1, 3, 0, 1, 0, 3, 
	3, 1, 3, 1, 0, 3, 3, 1, 
	3
};

static const short _yacl_parser_index_offsets[] = {
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
	222, 226, 232, 236, 242, 246, 251, 253, 
	257, 260, 266, 273, 277, 283, 286, 292, 
	297, 305, 313, 318, 325, 328, 334, 339, 
	347, 355, 360, 367, 373, 378, 386, 394, 
	399
};

static const unsigned char _yacl_parser_indicies[] = {
	1, 2, 3, 4, 5, 0, 6, 7, 
	0, 8, 0, 9, 0, 10, 0, 11, 
	0, 12, 0, 13, 0, 14, 0, 15, 
	16, 17, 0, 18, 0, 19, 0, 20, 
	0, 21, 22, 11, 0, 22, 0, 23, 
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
	0, 21, 22, 11, 0, 88, 0, 89, 
	0, 21, 22, 11, 0, 90, 0, 91, 
	0, 92, 0, 11, 0, 93, 0, 94, 
	0, 95, 0, 14, 0, 96, 0, 97, 
	0, 98, 0, 14, 0, 99, 0, 100, 
	0, 101, 0, 102, 0, 14, 0, 0, 
	103, 104, 105, 106, 107, 0, 108, 30, 
	11, 0, 109, 110, 111, 112, 113, 0, 
	114, 38, 115, 0, 116, 117, 118, 119, 
	120, 0, 121, 46, 122, 0, 123, 124, 
	125, 126, 0, 11, 0, 124, 125, 127, 
	0, 128, 11, 0, 124, 125, 129, 129, 
	129, 0, 123, 124, 125, 130, 130, 130, 
	0, 123, 123, 131, 0, 123, 123, 132, 
	132, 132, 0, 121, 46, 0, 133, 117, 
	118, 119, 134, 0, 121, 135, 46, 122, 
	0, 133, 117, 118, 119, 136, 136, 136, 
	0, 116, 117, 118, 119, 137, 137, 137, 
	0, 116, 118, 116, 138, 0, 116, 118, 
	116, 139, 139, 139, 0, 114, 38, 0, 
	140, 110, 111, 112, 141, 0, 114, 142, 
	38, 115, 0, 140, 110, 111, 112, 143, 
	143, 143, 0, 109, 110, 111, 112, 144, 
	144, 144, 0, 109, 111, 109, 145, 0, 
	109, 111, 109, 146, 146, 146, 0, 147, 
	104, 105, 106, 148, 0, 108, 149, 30, 
	11, 0, 147, 104, 105, 106, 150, 150, 
	150, 0, 103, 104, 105, 106, 151, 151, 
	151, 0, 103, 105, 103, 152, 0, 103, 
	105, 103, 153, 153, 153, 0, 0
};

static const char _yacl_parser_trans_targs[] = {
	0, 2, 61, 65, 69, 73, 3, 7, 
	4, 5, 6, 78, 8, 9, 10, 11, 
	55, 58, 12, 13, 14, 15, 16, 17, 
	17, 52, 52, 18, 79, 51, 20, 21, 
	21, 46, 46, 22, 81, 45, 24, 25, 
	25, 40, 40, 26, 83, 39, 28, 29, 
	29, 34, 34, 30, 85, 33, 87, 32, 
	89, 90, 35, 91, 36, 92, 94, 38, 
	96, 97, 41, 98, 42, 99, 101, 44, 
	103, 104, 47, 105, 48, 106, 107, 50, 
	109, 110, 53, 111, 54, 112, 56, 57, 
	59, 60, 62, 63, 64, 66, 67, 68, 
	70, 71, 72, 74, 75, 76, 77, 80, 
	49, 20, 108, 79, 19, 82, 43, 24, 
	102, 81, 23, 100, 84, 37, 28, 95, 
	83, 27, 93, 86, 31, 88, 85, 87, 
	31, 89, 90, 91, 92, 27, 94, 37, 
	96, 97, 98, 99, 23, 101, 43, 103, 
	104, 105, 106, 19, 107, 49, 109, 110, 
	111, 112
};

static const char _yacl_parser_trans_actions[] = {
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
	17, 17, 17, 0, 0, 17, 17, 17, 
	17, 0, 0, 0, 17, 17, 17, 17, 
	0, 0, 0, 17, 17, 17, 0, 0, 
	0, 0, 0, 0, 0, 17, 0, 0, 
	0, 0, 0, 0, 17, 0, 0, 0, 
	0, 0, 0, 17, 0, 0, 0, 0, 
	0, 0
};

static const char _yacl_parser_to_state_actions[] = {
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

static const char _yacl_parser_from_state_actions[] = {
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

static const char _yacl_parser_eof_actions[] = {
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
	0, 17, 0, 17, 0, 17, 0, 17, 
	0, 17, 17, 17, 17, 0, 17, 0, 
	17, 17, 17, 17, 0, 17, 0, 17, 
	17, 17, 17, 17, 0, 17, 17, 17, 
	17
};

static const int yacl_parser_start = 1;
static const int yacl_parser_first_final = 78;
static const int yacl_parser_error = 0;

static const int yacl_parser_en_main = 1;


#line 150 "yacl/ragel/parser.rl"


yacl_error_t parser(char* p, char* pe, walk_stack_s* stack)
{
    int cs = 0;
    
#line 304 "yacl/parser/parser.c"
	{
	cs = yacl_parser_start;
	}

#line 156 "yacl/ragel/parser.rl"

    stack->bufr_begin = p;

    option_data_stack_s* option;

    char* eof = pe;

    
#line 318 "yacl/parser/parser.c"
	{
	int _klen;
	const char *_keys;
	int _trans;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	switch ( _yacl_parser_from_state_actions[cs] ) {
	case 10:
#line 78 "yacl/ragel/parser.rl"
	{
                            option->base[option->idx] = 16;
                        }
	break;
#line 336 "yacl/parser/parser.c"
	}

	_keys = _yacl_parser_trans_keys + _yacl_parser_key_offsets[cs];
	_trans = _yacl_parser_index_offsets[cs];

	_klen = _yacl_parser_single_lengths[cs];
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

	_klen = _yacl_parser_range_lengths[cs];
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
	_trans = _yacl_parser_indicies[_trans];
	cs = _yacl_parser_trans_targs[_trans];

	if ( _yacl_parser_trans_actions[_trans] == 0 )
		goto _again;

	switch ( _yacl_parser_trans_actions[_trans] ) {
	case 5:
#line 29 "yacl/ragel/parser.rl"
	{
                            stack->valid_options[OPT_DATA] = true;
                            option = &stack->options[OPT_DATA];
                        }
	break;
	case 6:
#line 34 "yacl/ragel/parser.rl"
	{
                            stack->valid_options[OPT_REG] = true;
                            option = &stack->options[OPT_REG];
                        }
	break;
	case 7:
#line 39 "yacl/ragel/parser.rl"
	{
                            stack->valid_options[OPT_ADDR] = true;
                            option = &stack->options[OPT_ADDR];
                        }
	break;
	case 8:
#line 44 "yacl/ragel/parser.rl"
	{
                            stack->valid_options[OPT_STATE] = true;
                            option = &stack->options[OPT_STATE];
                        }
	break;
	case 9:
#line 49 "yacl/ragel/parser.rl"
	{
                            if (option->idx >= 32)
                            { 
                                uint32_t bufr_idx = p - stack->bufr_begin;
                                yacl_printf("parse error at index %u. too many arguments\n", bufr_idx);
                                return YACL_PARSE_BAD;
                            }

                            option->args[option->idx] = p;
                        }
	break;
	case 17:
#line 60 "yacl/ragel/parser.rl"
	{
                            (*p) = '\0'; 
                            
                            uint32_t max_str_len = 10;

                            if (option->base[option->idx] == 16)
                                max_str_len = 8;

                            if (strlen(option->args[option->idx]) > max_str_len)
                            { 
                                uint32_t bufr_idx = p - stack->bufr_begin;
                                yacl_printf("parse error at index %u. argument too large\n", bufr_idx);
                                return YACL_PARSE_BAD;
                            }

                            ++option->idx;
                        }
	break;
	case 1:
#line 84 "yacl/ragel/parser.rl"
	{
                        uint32_t bufr_idx = p - stack->bufr_begin;
                        yacl_printf("parse error at index %u. misspelled or unknown argument\n", bufr_idx);
                        return YACL_PARSE_BAD;
                    }
	break;
#line 464 "yacl/parser/parser.c"
	}

_again:
	switch ( _yacl_parser_to_state_actions[cs] ) {
	case 16:
#line 14 "yacl/ragel/parser.rl"
	{ stack->action = ACTION_WRITE;}
	break;
	case 15:
#line 15 "yacl/ragel/parser.rl"
	{ stack->action = ACTION_READ; }
	break;
	case 14:
#line 16 "yacl/ragel/parser.rl"
	{ stack->action = ACTION_PLOT; }
	break;
	case 3:
#line 17 "yacl/ragel/parser.rl"
	{ stack->action = ACTION_CONF; }
	break;
	case 13:
#line 18 "yacl/ragel/parser.rl"
	{ stack->action = ACTION_HELP; }
	break;
	case 2:
#line 19 "yacl/ragel/parser.rl"
	{ stack->action = ACTION_CLEAR;}
	break;
	case 4:
#line 23 "yacl/ragel/parser.rl"
	{ stack->stream = STREAM_GPIO; }
	break;
	case 11:
#line 24 "yacl/ragel/parser.rl"
	{ stack->stream = STREAM_I2C;  }
	break;
	case 12:
#line 25 "yacl/ragel/parser.rl"
	{ stack->stream = STREAM_SPI;  }
	break;
#line 505 "yacl/parser/parser.c"
	}

	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	switch ( _yacl_parser_eof_actions[cs] ) {
	case 17:
#line 60 "yacl/ragel/parser.rl"
	{
                            (*p) = '\0'; 
                            
                            uint32_t max_str_len = 10;

                            if (option->base[option->idx] == 16)
                                max_str_len = 8;

                            if (strlen(option->args[option->idx]) > max_str_len)
                            { 
                                uint32_t bufr_idx = p - stack->bufr_begin;
                                yacl_printf("parse error at index %u. argument too large\n", bufr_idx);
                                return YACL_PARSE_BAD;
                            }

                            ++option->idx;
                        }
	break;
	case 1:
#line 84 "yacl/ragel/parser.rl"
	{
                        uint32_t bufr_idx = p - stack->bufr_begin;
                        yacl_printf("parse error at index %u. misspelled or unknown argument\n", bufr_idx);
                        return YACL_PARSE_BAD;
                    }
	break;
#line 544 "yacl/parser/parser.c"
	}
	}

	_out: {}
	}

#line 164 "yacl/ragel/parser.rl"

    return YACL_SUCCESS;
}