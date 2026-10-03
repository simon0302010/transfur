#include "ui_core.h"

void ui_init(struct ui_context *ctx, struct widget *widgets, size_t count) {
        ctx->widgets = widgets;
        ctx->count = count;
        ctx->focus_index = -1;
        ctx->running = true;
}

tbool ui_handle_event(struct ui_context *ctx, const struct ui_event *event) {
        /* TODO handle event */
}

void ui_update(struct ui_context *ctx, double dt) {
        /* TODO update */
}
