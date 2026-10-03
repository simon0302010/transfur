#include "app.h"
#include "../core/widget.h"
#include "../core/widgets/text.h"
#include "../core/widgets/loading_bar.h"

#define APP_WIDGET_COUNT 2

static struct widget widgets[APP_WIDGET_COUNT];
static char title[50] = "Hello, welcome to Transfur.";
static struct widget_loading_bar_options loading_bar_opts;

void app_init(struct ui_context *ctx) {
        loading_bar_opts.title = title;
        loading_bar_opts.speed = 2.0f;
        loading_bar_opts.offset = 0;
        loading_bar_opts.accumulator = 0.0f;

        widgets[0] = widget_text(50, title);

        

        widgets[1] = widget_loading_bar(&loading_bar_opts, sizeof(loading_bar_opts));

        ctx->widgets = widgets;
        ctx->count = APP_WIDGET_COUNT;
}
