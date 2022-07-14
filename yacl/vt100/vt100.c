#include "vt100.h"

// https://tintin.mudhalla.net/info/vt100/
// https://vt100.net/docs/vt102-ug/appendixc.html
// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Application-Program-Command-functions

extern usr_printf_f yacl_printf;
extern usr_snprintf_f yacl_snprintf;

void vt100_welcome()
{
	vt100_erase_current_line();
	vt100_clear_display();
    yacl_printf("\x1b\x63YACL by Nick\n\n\rExplore peripherals connected to your MCU freely!\n\rType 'help' for more information or visit my GitHub at NicolasRodriguez676\n\n\r>> ");
}

void vt100_yacl_view()
{
	yacl_printf("\n\r>> ");
}

void vt100_report_exact_error(walk_stack_s* stack, yacl_error_e error)
{
	if (stack->bufr_err_beg + 1 == stack->bufr_err_idx)
		yacl_printf("\x1b[1B\x1b[%uC\x1b[32m~%s\n%s\n", stack->bufr_err_beg + 3, "\x1b[31m^\x1b[0m\n\r", yacl_error_desc(error));
	else if (stack->bufr_err_beg == stack->bufr_err_idx)
		yacl_printf("\x1b[1B\x1b[%uC\x1b[32m~%s\n%s\n", stack->bufr_err_beg + 2, "\x1b[31m^\x1b[0m\n\r", yacl_error_desc(error));
	else
		yacl_printf("\x1b[1B\x1b[%uC\x1b[32m~\x1b[%ub%s\n%s\n", stack->bufr_err_beg + 4, (stack->bufr_err_idx - 1) - (stack->bufr_err_beg + 1), "\x1b[31m^\x1b[0m\n\r", yacl_error_desc(error));
}

void vt100_clear_display()
{
    yacl_printf("\x1b[2J");
}

void vt100_erase_current_line()
{
	yacl_printf("\x1b[2K");
}

void vt100_backspace()
{
	yacl_printf("\x1b[1P");
}

void vt100_cursor_save()
{
	yacl_printf("\x1b\x37");
}

void vt100_cursor_restore()
{
	yacl_printf("\x1b\x37");
}

void vt100_cursor_column_begin()
{
	yacl_printf("\x1b[1G");
}

void vt100_cursor_upward(uint8_t steps)
{
	yacl_printf("\x1b[%uA", steps);
}

void vt100_cursor_downward(uint8_t steps)
{
	yacl_printf("\x1b[%uB", steps);
}

void vt100_cursor_backward(uint8_t steps)
{
	yacl_printf("\x1b[%uD", steps);
}

void vt100_cursor_forward(uint8_t steps)
{
	yacl_printf("\x1b[%uC", steps);
}

float get_step_size(yacl_graph_s* graph)
{
    float step_size = (graph->upper_range - graph->lower_range) / (float)graph->num_steps;

    if (step_size < 0.0f)
        return -1.0f * step_size;

    return step_size;
}

void vt100_draw_graph(yacl_graph_s* graph)
{
	vt100_clear_display();

	// top
	yacl_printf("\x1b\x63\x1b[1B\x1b[35m\xe2\x94\x8f\xe2\x94\x81\x1b[%ub\xe2\x94\x93", graph->num_samples);
	
	// middle
	float step_size = get_step_size(graph) - 0.00001f; // ensure zero is shown as zero. not negative zero
	float label_val = graph->upper_range;
	
	for (uint32_t i = 0; i <= graph->num_steps; ++i)
	{
		yacl_printf("\x1b[1G\x1b[1B\xe2\x94\x83\x20\x1b[%ub\xe2\x94\x83 %.3f%s", graph->num_samples, label_val, graph->units);
		label_val -= step_size;
	}
	
	// bottom
	yacl_printf("\x1b[1G\x1b[1B\xe2\x94\x97\xe2\x94\x81\x1b[%ub\xe2\x94\x9b\x1b[0m", graph->num_samples);
	
	// 2 down, 1 right -> origin (2D, 1R)
	yacl_printf("\x1b[1;1f");
}

void vt100_plot_graph(yacl_graph_s* graph, float data)
{
    float step_size = get_step_size(graph);
    float label_val = graph->lower_range;
    uint32_t column = graph->num_steps;

    do
    {
        if (label_val >= data)
            break;
        label_val += step_size;

    } while (--column);
	
	yacl_printf("\x1b[36m");
	
	if (graph->display_count > graph->num_samples)
	{
		for (uint32_t i = 0; i <= graph->num_steps; ++i)
		{
			// erase left side. pulls the graph to the left
			// go the right to insert the data point
			yacl_printf("\x1b[%uB\x1b[1C\x1b[1P\x1b[%uC", 2 + i, graph->num_samples - 1);
			
			// data point only happens once
			if (i == column)
				yacl_printf("\xe2\x97\x8f");
			
			// push right side labels to the right
			// reset cursor to (1,1)
			yacl_printf("\x1b[1@\x1b[1;1f");
		}
	}
	else
	{
		yacl_printf("\x1b[%uB\x1b[%uC\xe2\x97\x8f\x1b[1;1f", 2 + column, 1 + graph->display_count);
		++graph->display_count;
	}
}

void vt100_end_plot(yacl_graph_s* graph)
{
    graph->display_count = 0;
    yacl_printf("\x1b[0m\x1b[%uB\x1b[1G", graph->num_steps + 3);
}
