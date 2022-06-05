#include "vt100.h"

// https://tintin.mudhalla.net/info/vt100/

enum VT_100_Control_Sequences {
	ESC = 0x1b,
	CSI = 0x5b
};

void (*print_func)(char data);

void vt100_rst_term()
{
	print_func(ESC);
	print_func('c');
}

void vt100_backspace()
{
	print_func(ESC);
	print_func(CSI);
	print_func(1);
	print_func('X');
}
