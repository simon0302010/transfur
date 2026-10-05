#include "button.h"

struct widget widget_button(struct widget_button_options *options,
                                      size_t size) {
        struct widget new_button;

        new_button.type = WIDGET_BUTTON;

        new_button.content_size = size;

        new_button.content = options;

        return new_button;
}
