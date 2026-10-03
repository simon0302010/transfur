#include "loading_bar.h"

struct widget widget_loading_bar(struct widget_loading_bar_options *options,
                                 size_t size) {
        struct widget new_loading_bar;

        new_loading_bar.type = WIDGET_LOADING_BAR;

        new_loading_bar.content_size = size;

        new_loading_bar.content = options;

        return new_loading_bar;
}

void widget_update_loading_bar(struct widget *widget, double dt) {
        struct widget_loading_bar_options *options = widget->content;
        float step_interval;
        if (options->speed <= 0.0f) {
                return;
        }
        step_interval = 1.0f / options->speed;
        options->accumulator += (float)dt;
        if (options->accumulator >= step_interval) {
                int steps = (int)(options->accumulator / step_interval);
                options->offset = (options->offset + steps) %
                                  6; /* TODO: replace 6 with charset_size */
                options->accumulator -= (float)steps * step_interval;
        }
}
