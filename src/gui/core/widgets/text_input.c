#include "text_input.h"
#include "../../../misc/bool.h"
#include <string.h>

struct widget widget_text_input(struct widget_text_input_options *options,
                                size_t size) {
        struct widget new_text_input;

        new_text_input.type = WIDGET_TEXT_INPUT;

        new_text_input.content_size = size;

        new_text_input.content = options;

        new_text_input.is_focused = false;

        return new_text_input;
}

tbool widget_handle_event_text_input(struct widget *widget,
                                     struct ui_event *event) {
        struct widget_text_input_options *options = widget->content;

        if (event->type == UI_EVENT_KEY_DOWN) {
                switch (event->key) {
                case UI_KEY_CHAR: {
                        tbool is_last = false;

                        if (options->cursor + 1 >= options->max_len) {
                                return true;
                        }

                        if (options->buffer[options->cursor] == '\0') {
                                is_last = true;
                        }

                        options->buffer[options->cursor] = event->ch;
                        options->cursor++;
                        if (is_last) {
                                options->buffer[options->cursor] = '\0';
                        }
                        return true;
                }
                case UI_KEY_BACKSPACE:
                        if (options->cursor > 0) {
                                options->buffer[options->cursor - 1] = '\0';
                                options->cursor--;
                        }
                        /* TODO: shift all chars left */
                        return true;
                case UI_KEY_DELETE:
                        break;
                case UI_KEY_LEFT:
                        if (options->cursor > 0) {
                                options->cursor--;
                        }
                        return true;
                case UI_KEY_RIGHT:
                        if (options->cursor < strlen(options->buffer)) {
                                options->cursor++;
                        }
                        return true;
                }
        }

        return false;
}