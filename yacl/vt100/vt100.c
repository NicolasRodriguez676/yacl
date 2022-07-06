#include "vt100.h"

// https://tintin.mudhalla.net/info/vt100/
// https://vt100.net/docs/vt102-ug/appendixc.html
// https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Application-Program-Command-functions

enum Plot_Buffer_Sizes {
    NUM_VERT_LABELS = 48
};

extern usr_printf_t yacl_printf;
extern usr_snprintf_t yacl_snprintf;

static const char* vt100_esc     = "\x1b";
static const char* vt100_sve_csr = "\x1b" "7";
static const char* vt100_res_csr = "\x1b" "8";

static uint32_t display_count = 0;

void vt100_welcome()
{
    vt100_rst_term();
    yacl_printf("YACL by Nick\n\n\rExplore peripherals connected to your MCU freely!\n\rType 'help' for more information or visit my GitHub\n\n\r>> ");
}

void vt100_rst_term()
{
    yacl_printf("\x1b[2J");
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
    char* vt100_bk_sp = "\x1b[1P";
    char* vt100_csr_bk = "\x1b[1D";

    yacl_printf("%s%s", vt100_csr_bk, vt100_bk_sp);
}

float get_step_size(yacl_graph_t* graph)
{
    float step_size = (graph->upper_range - graph->lower_range) / (float)graph->num_steps;

    if (step_size < 0.0f)
        return -1.0f * step_size;

    return step_size;
}

void vt100_draw_graph(yacl_graph_t* graph)
{
    yacl_printf("\n\r");

    char graph_fill_blanks[24] = { '\0' };
    yacl_snprintf(graph_fill_blanks, sizeof(graph_fill_blanks), "\x1b[1D\x1b[%u@\x1b[%uC", graph->num_samples, graph->num_samples + 1);

    char graph_vt_labels[NUM_VERT_LABELS][8] = { {'\0'} };

    float step_size = get_step_size(graph);
    float label_val = graph->lower_range;

    for (uint32_t i = 0; i <= graph->num_steps; ++i)
    {
        yacl_snprintf(graph_vt_labels[i], sizeof(graph_vt_labels) / sizeof(graph_vt_labels[0][0]), "%.3f", label_val);
        label_val += step_size;
    }

    yacl_printf(" ");
    for (uint8_t i = 0; i < graph->num_samples; ++i)
        yacl_printf("_");

    yacl_printf("\n\r|%s7|%s %s%s\n\r", vt100_esc, graph_fill_blanks, graph_vt_labels[graph->num_steps], graph->units);

    for (uint32_t i = 1; i <= graph->num_steps; ++i)
        yacl_printf("||%s %s%s\n\r", graph_fill_blanks,  graph_vt_labels[(graph->num_steps) - i], graph->units);

    yacl_printf(" ");
    for (uint8_t i = 0; i < graph->num_samples; ++i)
        yacl_printf("-");
}

void vt100_plot_graph(yacl_graph_t* graph, float data)
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

    if (display_count == graph->num_samples)
    {
        --display_count;
        char move_display[48] = { '\0' };

        yacl_snprintf(move_display, sizeof(move_display)/sizeof(move_display[0]), "\r\x1b[1C\x1b[1P\x1b[%uC\x1b[1P\x1b[2@\x1b[1B", graph->num_samples - 2);

        yacl_printf("%s", vt100_res_csr);

        for (uint32_t i = 0; i <= graph->num_steps; ++i)
            yacl_printf("%s", move_display);

        yacl_printf("\x1b[%uA%s", graph->num_steps + 1, vt100_sve_csr);
    }

    if (!column)
    {
        yacl_printf("%s8*%s7", vt100_esc, vt100_esc);
        ++display_count;
        return;
    }

    char plot_mv_vt_dw[8] = { 0 };
    char plot_mv_vt_up[8] = { 0 };

    yacl_snprintf(plot_mv_vt_dw, sizeof(plot_mv_vt_dw), "\x1b[%uB", column);
    yacl_snprintf(plot_mv_vt_up, sizeof(plot_mv_vt_up), "\x1b[%uA", column);

    yacl_printf("%s8%s*%s%s7", vt100_esc, plot_mv_vt_dw, plot_mv_vt_up, vt100_esc);

    ++display_count;
}

void vt100_end_plot(yacl_graph_t* graph)
{
    display_count = 0;
    yacl_printf("\x1b[%uB\n\n", graph->num_steps + 1);
}
