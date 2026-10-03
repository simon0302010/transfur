#include "render.h"
#include "widgets/text.h"

void tui_render(struct ui_context *ctx) {
        int i;

        for (i = 0; i < ctx->count; i++) {
                switch (ctx->widgets[i].type) {
                case WIDGET_TEXT:
                        tui_render_text_widget(ctx->widgets[i]);
                        break;
                default:
                        break;
                }
        }
}
