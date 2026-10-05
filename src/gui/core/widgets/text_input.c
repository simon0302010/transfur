#include "text_input.h"

struct widget widget_text_input(struct widget_text_input_options *options,
                                size_t size) {
        struct widget new_text_input;

        new_text_input.type = WIDGET_TEXT_INPUT;

        new_text_input.content_size = size;

        new_text_input.content = options;

        return new_text_input;
}

tbool widget_handle_event_text_input(struct widget *widget,
                                     struct ui_event *event) {
        struct widget_text_input_options *options = widget->content;

        if (event->type == UI_EVENT_KEY_DOWN) {
                if (event->key == UI_KEY_CHAR) {
                        if (options->cursor + 1 >= options->max_len) {
                                return true;
                        }

                        options->buffer[options->cursor] = event->ch;
                        options->cursor++;
                        options->buffer[options->cursor] = '\0';
                        return true;
                } else if (event->key == UI_KEY_BACKSPACE) {
                        options->buffer[options->cursor - 1] = '\0';
                        options->cursor--;
                        /* TODO: shift all chars left */
                }
        }

        return false;
}