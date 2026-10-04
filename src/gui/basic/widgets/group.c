#include "group.h"

int basic_render_group_widget(struct widget widget,
                              struct basic_render_details details) {
        int i;
        int height = 0;

        struct widget *children = widget.content;

        for (i = 0; i < widget.content_size; i++) {
                int increase = basic_render_widget(children[i], details);
                height += increase;
                details.y += increase;
        }

        return height;
}
