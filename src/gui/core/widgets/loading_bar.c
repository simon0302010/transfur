#include "loading_bar.h"

struct widget widget_loading_bar(struct widget_loading_bar_options *options,
                                 size_t size) {
        struct widget new_loading_bar;

        new_loading_bar.type = WIDGET_LOADING_BAR;

        new_loading_bar.content_size = size;

        new_loading_bar.content = options;

        return new_loading_bar;
}