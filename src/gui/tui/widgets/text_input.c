#include "../../core/widgets/text_input.h"
#include "../ansi.h"
#include "../text.h"
#include "loading_bar.h"
#include <malloc.h>
#include <string.h>

char **tui_render_text_input_widget(struct widget widget, int width) {
        struct widget_text_input_options *options = widget.content;

        size_t label_size = strlen(options->label);
        size_t buffer_size = strlen(options->buffer);

        /* label + space + [_] + buffer + null terminator */
        char *text = malloc(label_size + 1 + 3 + options->max_len + 1);

        /* TODO: Account for cursor position */
        strcpy(text, options->label);
        text[label_size] = ' ';
        text[label_size + 1] = '[';
        strcpy(text, options->buffer);
        if (widget.is_focused) {
                text[label_size + 2 + buffer_size] = '_';
                text[label_size + 3 + buffer_size] = ']';
        } else {
                text[label_size + 2 + buffer_size] = ']';
        }

        return wrap_text(text, width);
}
