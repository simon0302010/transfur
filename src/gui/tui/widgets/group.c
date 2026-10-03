#include "../../core/widget.h"
#include "../render.h"

void tui_render_group_widget(struct widget widget, int width) {
        int i;

        struct widget *children = widget.content;

        for (i = 0; i < widget.content_size; i++) {
                tui_render_widget(children[i], width);
        }
}
