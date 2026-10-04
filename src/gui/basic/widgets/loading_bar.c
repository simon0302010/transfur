#include "../../core/widgets/loading_bar.h"
#include "../sdl3/basic_wrapper.h"
#include "../text.h"
#include "loading_bar.h"
#include <string.h>

#define LOADING_SEGMENT_SIZE 8

static const struct color color_set[6] = {
    {255, 0, 0},   /* red */
    {20, 255, 0},  /* green */
    {0, 0, 255},   /* blue */
    {0, 255, 255}, /* cyan */
    {255, 0, 255}, /* magenta */
    {255, 255, 0}  /* yellow */
};

static struct color text_color = {255, 255, 255};

int basic_render_loading_bar_widget(struct widget widget,
                                    struct basic_render_details details) {
        struct widget_loading_bar_options *options = widget.content;

        short i;
        short color_set_size = 6;

        int bar_width = details.width - 3;
        int title_width;

        if (bar_width <= 0)
                return 0;

        title_width = draw_text(options->title, details.x, details.y, 1, text_color).width;
        bar_width -= title_width;

        for (i = 0; i < bar_width; i += 1)
                basic_fill_rect(
                    title_width + 3 + details.x + i * LOADING_SEGMENT_SIZE, details.y,
                    LOADING_SEGMENT_SIZE, LOADING_SEGMENT_SIZE,
                    color_set[(i + options->offset) % color_set_size]);

        return LOADING_SEGMENT_SIZE;
}