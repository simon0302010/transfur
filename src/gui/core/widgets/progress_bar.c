#include "progress_bar.h"

struct widget widget_progress_bar(struct widget_progress_bar_options *options,
                                      size_t size) {
        struct widget new_progress_bar;

        new_progress_bar.type = WIDGET_PROGRESS_BAR;

        new_progress_bar.content_size = size;

        new_progress_bar.content = options;

        return new_progress_bar;
}
