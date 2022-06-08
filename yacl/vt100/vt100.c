#include "vt100.h"

// https://tintin.mudhalla.net/info/vt100/


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
