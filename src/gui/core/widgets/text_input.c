#include "text_input.h"

struct widget widget_text_input(struct widget_text_input_options *options,
                                size_t size) {
        struct widget new_text_input;

        new_text_input.type = WIDGET_TEXT_INPUT;

        new_text_input.content_size = size;

        new_text_input.content = options;

        return new_text_input;
}