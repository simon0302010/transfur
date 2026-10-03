#include "ui_core.h"

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
        /* TODO update */
}
