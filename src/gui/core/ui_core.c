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
                        struct widget_loading_bar_options *options =
                            ctx->widgets[i].content;
                        float step_interval;
                        if (options->speed <= 0.0f) {
                                break;
                        }
                        step_interval = 1.0f / options->speed;
                        options->accumulator += (float)dt;
                        while (options->accumulator >= step_interval) {
                                options->offset =
                                    (options->offset + 1) %
                                    6; /* TODO: replace 6 with charset_size */
                                options->accumulator -= step_interval;
                        }
                        break;
                }
                default:
                        break;
                }
        }
}
