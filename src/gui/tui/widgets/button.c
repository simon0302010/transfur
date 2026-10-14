#include "../../core/widgets/button.h"
#include "../text.h"
#include "button.h"
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

char **tui_render_button_widget(struct widget widget, int width) {
        struct widget_button_options *options = widget.content;

        int length = strlen(options->title);

        char *text = malloc(length + 4 + 1);

        /* TODO: add ANSI support (if available) for button styles */

        /* add starting brackets & arrows if focuses */
        if (widget.is_focused) {
                text[0] = '>';
                text[1] = '{';
                strcpy(text + 2, options->title);
                text[length + 2] = '}';
                text[length + 3] = '<';
                text[length + 4] = '\0';
        } else {
                text[0] = '{';
                text[1] = ' ';
                strcpy(text + 2, options->title);
                text[length + 2] = ' ';
                text[length + 3] = '}';
                text[length + 4] = '\0';
        }

        return wrap_text(text, width);
}
