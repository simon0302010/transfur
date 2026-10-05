#ifndef CORE_WIDGET_H
#define CORE_WIDGET_H

#include "../../misc/bool.h"
#include <stddef.h>

enum widget_type {
        WIDGET_TEXT,
        WIDGET_PROGRESS_BAR,
        WIDGET_LOADING_BAR,
        WIDGET_TEXT_INPUT,
        WIDGET_GROUP, /* this will replace BASIC_PANEL */
        WIDGET_BUTTON
};

struct widget {
        enum widget_type type;
        tbool is_focused;
        size_t content_size;
        void *content;
};

#endif /* CORE_WIDGET_H */