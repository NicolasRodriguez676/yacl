#include "vt100.h"
#include "../yacl.h"

// https://tintin.mudhalla.net/info/vt100/

extern usr_printf_t yacl_printf;
//extern int (* yacl_snprintf)(char* buffer, size_t count, const char* format, ...);


void vt100_rst_term()
{
    char* vt100_rst = "\x1b[2J";
    yacl_printf("%s", vt100_rst);
}

void vt100_yacl_view()
{
    yacl_printf("\r>> ");
}

void vt100_error(const char* error_str)
{
    yacl_printf("\n\rERROR:: %s\n\n\r", error_str);

    vt100_yacl_view();
}

void vt100_error_data(const char* error_str, uint32_t error_data)
{
    yacl_printf("\n\rERROR:: %s - %d\n\n\r", error_str, error_data);

    vt100_yacl_view();
}

void vt100_backspace()
{
    char* vt100_bk_sp = "\x1b[1D";
    char* vt100_csr_bk = "\x1b[1P";

    yacl_printf("%s%s", vt100_bk_sp, vt100_csr_bk);
}
