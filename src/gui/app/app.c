#include "app.h"
#include "../core/widget.h"
#include "../core/widgets/text.h"

#define APP_WIDGET_COUNT 1

static struct widget widgets[APP_WIDGET_COUNT];
static char title[50] = "Hello, welcome to Transfur.";

void app_init(struct ui_context *ctx) {
        widgets[0] = widget_text(50, title);

        ctx->widgets = widgets;
        ctx->count = APP_WIDGET_COUNT;
}
