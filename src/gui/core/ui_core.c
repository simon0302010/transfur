#include "ui_core.h"
#include "ui_event.h"
#include "widget.h"
#include "widgets/button.h"
#include "widgets/loading_bar.h"
#include "widgets/text_input.h"

#define MAX_FOCUSABLE_WIDGETS 128

static void collect_focusable_widgets(struct widget *widgets, size_t count,
                                      struct widget **out_list,
                                      size_t *out_count, size_t max_count) {
        size_t i;

        if (widgets == NULL)
                return;

        for (i = 0; i < count; i++) {
                switch (widgets[i].type) {
                case WIDGET_GROUP:
                case WIDGET_SPLIT:
                        collect_focusable_widgets(
                            (struct widget *)widgets[i].content,
                            widgets[i].content_size, out_list, out_count,
                            max_count);
                        break;
                case WIDGET_TEXT_INPUT:
                case WIDGET_BUTTON:
                        if (*out_count < max_count) {
                                out_list[*out_count] = &widgets[i];
                                (*out_count)++;
                        }
                        break;
                default:
                        break;
                }
        }
}

static void update_widgets(struct widget *widgets, size_t count, double dt) {
        size_t i;

        if (widgets == NULL)
                return;

        for (i = 0; i < count; i++) {
                switch (widgets[i].type) {
                case WIDGET_LOADING_BAR:
                        widget_update_loading_bar(&(widgets[i]), dt);
                        break;
                case WIDGET_GROUP:
                case WIDGET_SPLIT:
                        update_widgets((struct widget *)widgets[i].content,
                                       widgets[i].content_size, dt);
                        break;
                default:
                        break;
                }
        }
}

void ui_init(struct ui_context *ctx) {
        ctx->focus_index = -1;
        ctx->focused_widget = NULL;
        ctx->running = true;
}

tbool ui_handle_event(struct ui_context *ctx, struct ui_event *event) {
        struct widget *focusable[MAX_FOCUSABLE_WIDGETS];
        size_t num_focusable = 0;
        size_t i;
        int current_idx = -1;
        int next_idx = -1;

        collect_focusable_widgets(ctx->widgets, ctx->count, focusable,
                                  &num_focusable, MAX_FOCUSABLE_WIDGETS);

        /* esc always unfocuses */
        if (event->type == UI_EVENT_KEY_DOWN && event->key == UI_KEY_ESC) {
                if (ctx->focused_widget != NULL) {
                        ctx->focused_widget->is_focused = false;
                        ctx->focused_widget = NULL;
                }
                for (i = 0; i < num_focusable; i++) {
                        focusable[i]->is_focused = false;
                }

                ctx->focus_index = -1;
                return true;
        }

        /* tab / shift+tab navigation */
        if (event->type == UI_EVENT_KEY_DOWN &&
            (event->key == UI_KEY_TAB || event->key == UI_KEY_SHIFT_TAB)) {
                if (num_focusable == 0) {
                        return true;
                }

                for (i = 0; i < num_focusable; i++) {
                        if (focusable[i]->is_focused) {
                                current_idx = (int)i;
                                focusable[i]->is_focused = false;
                        }
                }

                if (event->key == UI_KEY_TAB) {
                        if (current_idx == -1)
                                next_idx = 0;
                        else
                                next_idx = (current_idx + 1) % (int)num_focusable;
                } else {
                        if (current_idx == -1)
                                next_idx = (int)num_focusable - 1;
                        else
                                next_idx = (current_idx - 1 + (int)num_focusable) %
                                           (int)num_focusable;
                }

                focusable[next_idx]->is_focused = true;
                ctx->focus_index = next_idx;
                ctx->focused_widget = focusable[next_idx];
                return true;
        }

        /* if a widget if focused, check if it would handle interaction */
        if (ctx->focused_widget != NULL) {
                switch (ctx->focused_widget->type) {
                case WIDGET_TEXT_INPUT:
                        if (widget_handle_event_text_input(ctx->focused_widget,
                                                           event))
                                return true;
                        break;
                case WIDGET_BUTTON:
                        if (widget_handle_button_input(ctx->focused_widget,
                                                       event))
                                return true;
                        break;
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
        update_widgets(ctx->widgets, ctx->count, dt);
}
