#include "../../core/widgets/button.h"
#include "button.h"
#include <stdio.h>

void tui_render_button_widget(struct widget widget, int width) {
        struct widget_button_options *options = widget.content;

        /* TODO: should this be full width? */

        if (widget.is_focused) {
                /* TODO: Account for cursor position */
                printf(">{%s}<", options->title);

        } else {
                printf("{%s}", options->title);
        }
}