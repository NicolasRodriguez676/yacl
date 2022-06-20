#ifndef _VT100_H_
#define _VT100_H_

#include <stddef.h>
#include <stdint.h>


void vt100_rst_term();
void vt100_yacl_view();
void vt100_error(const char* error_str);
void vt100_error_data(const char* error_str, uint32_t error_data);
void vt100_backspace();

#endif //_VT100_H_
