#include "app.h"
#include "../core/widget.h"
#include "../core/widgets/text.h"

#define APP_WIDGET_COUNT 1

void app_init(struct ui_context *ctx) {
        struct widget widgets[APP_WIDGET_COUNT];
        size_t count = APP_WIDGET_COUNT;

        char title[50] = "Hello, welcome to Transfur.";

        widgets[0] = widget_text(50, title);

        ctx->widgets = widgets;
        ctx->count = count;
}
