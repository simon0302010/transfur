#ifndef CORE_WIDGETS_PROGRESS_BAR_H
#define CORE_WIDGETS_PROGRESS_BAR_H

#include "../widget.h"

struct widget_progress_bar_options {
        float *progress;
        char *title;
};

struct widget widget_progress_bar(struct widget_progress_bar_options *opts, size_t size);

#endif /* CORE_WIDGETS_PROGRESS_BAR_H */
