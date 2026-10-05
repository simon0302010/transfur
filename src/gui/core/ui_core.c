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

        /* tab / shift+tab navigation */
        if (event->type == UI_EVENT_KEY_DOWN &&
            (event->key == UI_KEY_TAB || event->key == UI_KEY_SHIFT_TAB)) {
                tbool found_widget = false;
                
                if (ctx->focus_index >= 0 && ctx->focus_index < ctx->count) {
                        ctx->widgets[ctx->focus_index].is_focused = false;
                }

                
                while (!found_widget) {
                        if (event->key == UI_KEY_TAB)
                                ctx->focus_index++;
                        else
                                ctx->focus_index--;

                        if (ctx->focus_index < 0) ctx->focus_index = ctx->count - 1;
                        if (ctx->focus_index >= ctx->count) ctx->focus_index = 0; 

                        switch (ctx->widgets[ctx->focus_index].type) {
                        case WIDGET_TEXT_INPUT:
                        case WIDGET_BUTTON: {
                                ctx->widgets[ctx->focus_index].is_focused =
                                    true;
                                found_widget = true;
                                break;
                        }
                        default:
                                break;
                        }
                }
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
