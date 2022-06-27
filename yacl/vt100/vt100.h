#ifndef _VT100_H_
#define _VT100_H_

#include <stddef.h>
#include <stdint.h>
#include "../yacl.h"


void vt100_rst_term();
void vt100_yacl_view();
void vt100_error(const char* error_str);
void vt100_error_data(const char* error_str, uint32_t error_data);
void vt100_backspace();

void vt100_draw_graph(yacl_graph_t* graph);
void vt100_plot_graph(yacl_graph_t *graph, float data);
void vt100_end_plot(yacl_graph_t *graph);

#endif //_VT100_H_
