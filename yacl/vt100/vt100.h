#ifndef _VT100_H_
#define _VT100_H_

#include <stddef.h>
#include <stdint.h>
#include "../yacl.h"

void vt100_welcome();
void vt100_rst_term();
void vt100_yacl_view();
void vt100_error(const char* error_str);
void vt100_error_data(const char* error_str, uint32_t error_data);
void vt100_csr_save();
void vt100_csr_restore();
void vt100_csr_col_one();
void vt100_csr_upward(uint8_t steps);
void vt100_csr_downward(uint8_t steps);
void vt100_csr_backward(uint8_t steps);
void vt100_csr_forward(uint8_t steps);
void v100_erase_current_line();
void vt100_backspace();

void vt100_draw_graph(yacl_graph_s* graph);
void vt100_plot_graph(yacl_graph_s *graph, float data);
void vt100_end_plot(yacl_graph_s *graph);

#endif //_VT100_H_
