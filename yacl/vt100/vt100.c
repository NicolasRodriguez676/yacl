#include "vt100.h"
#include "../yacl.h"

// https://tintin.mudhalla.net/info/vt100/

extern usr_printf_t yacl_printf;
//extern int (* yacl_snprintf)(char* buffer, size_t count, const char* format, ...);


void vt100_rst_term()
{
	char vt100_rst[3];

	vt100_rst[0] = ESC;
	vt100_rst[1] = 'c';
	vt100_rst[2] = '\0';

	yacl_printf("%s", vt100_rst);
}

void vt100_yacl_view()
{
	yacl_printf(">> ");
}

void vt100_error(const char* error_str)
{
	yacl_printf("\n%s\n\n", error_str);
}

void vt100_backspace()
{
	char vt100_bk_sp[5];

	vt100_bk_sp[0] = ESC;
	vt100_bk_sp[1] = CSI;
	vt100_bk_sp[2] = 1;
	vt100_bk_sp[3] = 'X';
	vt100_bk_sp[4] = '\0';

	yacl_printf("%s", vt100_bk_sp);
}
