#ifndef CORE_WIDGETS_LOADING_BAR_H
#define CORE_WIDGETS_LOADING_BAR_H

#include "../widget.h"

struct widget_loading_bar_options {
        char *title;
        size_t offset;
        float speed;
        float accumulator;
};

struct widget widget_loading_bar(struct widget_loading_bar_options *opts);

#endif /* CORE_WIDGETS_LOADING_BAR_H */
