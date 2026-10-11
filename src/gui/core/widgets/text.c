#include "text.h"

struct widget widget_text(int length, char *content) {
        struct widget new_text;

        new_text.type = WIDGET_TEXT;

        new_text.content_size = length + 1;

        new_text.content = content;

        new_text.is_focused = false;

        return new_text;
}