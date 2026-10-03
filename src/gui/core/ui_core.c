#include "ui_core.h"
#include "widget.h"
#include "widgets/loading_bar.h"

void ui_init(struct ui_context *ctx) {
        ctx->focus_index = -1;
        ctx->running = true;
}

tbool ui_handle_event(struct ui_context *ctx, const struct ui_event *event) {
        if (event->key == UI_KEY_CHAR && event->ch == 'q') {
                ctx->running = false;
        }
        /* TODO handle event */
}

void ui_update(struct ui_context *ctx, double dt) {
        int i;

        for (i = 0; i < ctx->count; i++) {
                switch (ctx->widgets[i].type) {
                case WIDGET_LOADING_BAR: {
                        widget_update_loading_bar(&(ctx->widgets[i]), dt);
                        break;
                }
                default:
                        break;
                }
        }
}
