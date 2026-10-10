#include "split.h"

int basic_render_split_widget(struct widget widget,
                              struct basic_render_details details) {
        int width_per_child;
        int i;
        int height = 0;
        struct widget *children;

        if (widget.content_size == 0)
                return 0;

        width_per_child = details.width / widget.content_size;
        children = widget.content;

        details.width = (int)width_per_child;

        for (i = 0; i < widget.content_size; i++) {
                int increase = basic_render_widget(children[i], details);
                if (increase > height)
                        height = increase;
                details.x += (int)width_per_child;
        }

        return height;
}
