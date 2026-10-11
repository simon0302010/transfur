#include "split.h"

struct widget widget_split(struct widget *children, size_t count) {
        struct widget new_group;

        new_group.type = WIDGET_SPLIT;

        new_group.content_size = count;

        new_group.content = children;

        new_group.is_focused = false;

        return new_group;
}