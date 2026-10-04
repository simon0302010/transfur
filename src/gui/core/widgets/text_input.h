#ifndef CORE_WIDGETS_TEXT_INPUT_H
#define CORE_WIDGETS_TEXT_INPUT_H

#include "../../../misc/bool.h"
#include "../ui_event.h"
#include "../widget.h"

struct widget_text_input_options {
        const char *label;
        char *buffer;
        size_t max_len;
        size_t cursor;
        tbool is_focused;
};

struct widget widget_text_input(struct widget_text_input_options *opts,
                                size_t size);

tbool widget_handle_event_text_input(struct widget *widget,
                                      struct ui_event *event);

#endif /* CORE_WIDGETS_TEXT_INPUT_H */
