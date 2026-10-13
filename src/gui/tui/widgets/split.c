#include "../../core/widget.h"
#include "../render.h"

char **tui_render_split_widget(struct widget widget, int width) {
        int i;
        int width_per_width = width / widget.content_size;

        struct widget *children = widget.content;

        for (i = 0; i < widget.content_size; i++) {
                tui_render_widget(children[i], width_per_width);
        }
}
