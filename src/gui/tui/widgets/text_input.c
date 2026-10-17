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
        char *text;
        char **output;

        /* label + space + '[' + buffer + cursor/bracket + null terminator */
        text = malloc(label_size + 1 + 1 + options->max_len + 3);

        /* TODO: Account for cursor position */
        strcpy(text, options->label);
        text[label_size] = ' ';
        text[label_size + 1] = '[';
        strcpy(text + label_size + 2, options->buffer);
        if (widget.is_focused) {
                text[label_size + 2 + buffer_size] = '_';
                text[label_size + 3 + buffer_size] = ']';
                text[label_size + 4 + buffer_size] = '\0';
        } else {
                text[label_size + 2 + buffer_size] = ']';
                text[label_size + 3 + buffer_size] = '\0';
        }

        output = wrap_text(text, width);
        free(text);
        return output;
}
