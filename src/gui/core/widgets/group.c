#include "group.h"

struct widget widget_group(struct widget *children, size_t count) {
        struct widget new_group;

        new_group.type = WIDGET_GROUP;

        new_group.content_size = count;

        new_group.content = children;

        return new_group;
}