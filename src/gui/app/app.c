#include "app.h"
#include "../core/widget.h"
#include "../core/widgets/button.h"
#include "../core/widgets/loading_bar.h"
#include "../core/widgets/text.h"
#include "../core/widgets/text_input.h"

#define APP_WIDGET_COUNT 4

static struct widget widgets[APP_WIDGET_COUNT];
static char title[50] = "Transfur TRANSFUR hello";
static char loading[] = "(this is a loading bar)";
static char input_buffer[50];
static struct widget_loading_bar_options loading_bar_opts;
static struct widget_text_input_options text_input_opts;
static struct widget_button_options button_opts;

void app_init(struct ui_context *ctx) {

        loading_bar_opts.title = loading;
        loading_bar_opts.speed = 2.0f;
        loading_bar_opts.offset = 0;
        loading_bar_opts.accumulator = 0.0f;

        text_input_opts.buffer = input_buffer;
        text_input_opts.max_len = 50;
        text_input_opts.label = title;
        text_input_opts.cursor = 0;

        button_opts.title = title;

        widgets[0] = widget_text(50, title);

        widgets[1] =
            widget_loading_bar(&loading_bar_opts, sizeof(loading_bar_opts));

        widgets[2] =
            widget_text_input(&text_input_opts, sizeof(text_input_opts));

        widgets[3] = widget_button(&button_opts, sizeof(button_opts));

        ctx->widgets = widgets;
        ctx->count = APP_WIDGET_COUNT;
}
