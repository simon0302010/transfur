#include "ui_core.h"
#include "ui_event.h"
#include "widget.h"
#include "widgets/loading_bar.h"
#include "widgets/text_input.h"

void ui_init(struct ui_context *ctx) {
        ctx->focus_index = -1;
        ctx->running = true;
}

tbool ui_handle_event(struct ui_context *ctx, struct ui_event *event) {
        /* esc always unfocuses */
        if (event->type == UI_EVENT_KEY_DOWN && event->key == UI_KEY_ESC) {
                ctx->focus_index = -1;
                return true;
        }

        /* if a widget if focused, check if it would handle interaction */
        if (ctx->focus_index >= 0 && ctx->focus_index < ctx->count) {
                struct widget focused_widget = ctx->widgets[ctx->focus_index];

                switch (focused_widget.type) {
                case WIDGET_TEXT_INPUT: {
                        if (widget_handle_event_text_input(&focused_widget,
                                                           event))
                                return true;
                }
                default:
                        break;
                }
        }

        if (event->type == UI_EVENT_KEY_DOWN && event->key == UI_KEY_CHAR &&
            event->ch == 'q') {
                ctx->running = false;
                return true;
        }

        return false;
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
