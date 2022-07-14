#ifndef _VT100_H_
#define _VT100_H_

#include <stddef.h>
#include <stdint.h>
#include "../yacl_types.h"

void vt100_welcome();
void vt100_yacl_view();
void vt100_report_exact_error(walk_stack_s* stack, yacl_error_e error);

void vt100_clear_display();
void vt100_erase_current_line();
void vt100_backspace();

void vt100_cursor_save();
void vt100_cursor_restore();
void vt100_cursor_column_begin();
void vt100_cursor_upward(uint8_t steps);
void vt100_cursor_downward(uint8_t steps);
void vt100_cursor_backward(uint8_t steps);
void vt100_cursor_forward(uint8_t steps);

void vt100_draw_graph(yacl_graph_s* graph);
void vt100_plot_graph(yacl_graph_s* graph, float data);
void vt100_end_plot(yacl_graph_s* graph);

#endif //_VT100_H_
