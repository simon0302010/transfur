#include "../../core/widgets/text_input.h"
#include "../ansi.h"
#include "loading_bar.h"
#include <malloc.h>

void tui_render_text_input_widget(struct widget widget, int width) {
        struct widget_text_input_options *options = widget.content;

        if (widget.is_focused) {
                /* TODO: Account for cursor position */
                printf("%s [%s_]", options->label, options->buffer);

        } else {
                printf("%s [%s]", options->label, options->buffer);
        }
}