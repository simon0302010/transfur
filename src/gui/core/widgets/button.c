#include "button.h"

struct widget widget_button(struct widget_button_options *options,
                            size_t size) {
        struct widget new_button;

        new_button.type = WIDGET_BUTTON;

        new_button.content_size = size;

        new_button.content = options;

        return new_button;
}

tbool widget_handle_button_input(struct widget *widget,
                                 struct ui_event *event) {
        struct widget_button_options *options = widget->content;

        if (event->type == UI_EVENT_KEY_DOWN) {
                if ((event->key == UI_KEY_CHAR && event->ch == ' ') ||
                    event->key == UI_KEY_ENTER) {
                        /* call the callback */
                        options->callback();
                }
        }

        return false;
}
