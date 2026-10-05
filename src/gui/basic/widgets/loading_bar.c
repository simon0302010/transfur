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

#define ENABLE_EASTER_EGGS

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

        title_width =
            draw_text(options->title, details.x, details.y, 1, text_color)
                .width;
        bar_width -= title_width;

#ifdef ENABLE_EASTER_EGGS
#define EASTER_EGG_SEGMENT_SIZE 12
        for (i = 0; i < bar_width; i += 1) {
                /* head position should be 0/7, 1/7, 2/7, 3/7, 4/7, 5/7, 6/7 */
                int head_position =
                    (int)((double)(options->offset) / 6.0 * bar_width);
                int j;
                struct color wheel_color = {20, 20, 40};

                basic_fill_rect(title_width + 3 + details.x + i, details.y + 2,
                                1, 2, text_color);
                basic_fill_rect(title_width + 3 + details.x + i,
                                details.y + EASTER_EGG_SEGMENT_SIZE - 4, 1, 2,
                                text_color);

                if ((i + 4) % 8 == 0)
                        basic_fill_rect(title_width + 3 + details.x + i,
                                        details.y, 2, EASTER_EGG_SEGMENT_SIZE,
                                        text_color);

                /* draw head */
                basic_fill_rect(title_width + 3 + details.x + head_position + 2,
                                details.y - 2, 2, 6, color_set[0]);
                basic_fill_rect(title_width + 3 + details.x + head_position,
                                details.y + 2, EASTER_EGG_SEGMENT_SIZE, 6,
                                color_set[0]);

                /* draw wheels */
                basic_fill_rect(title_width + 3 + details.x + head_position,
                                details.y + 7, 3, 3,
                                wheel_color);
                basic_fill_rect(title_width + 3 + details.x + head_position + 4,
                                details.y + 7, 3, 3,
                                wheel_color);
                basic_fill_rect(title_width + 3 + details.x + head_position + 8,
                                details.y + 7, 3, 3,
                                wheel_color);

                /* draw each car */
                for (j = 1; j < color_set_size; j++) {
                        int x = title_width + 3 + details.x + head_position -
                                (EASTER_EGG_SEGMENT_SIZE + 2) * j;
                        if (x < title_width + 3 + details.x)
                                x += bar_width;
                        basic_fill_rect(x, details.y, EASTER_EGG_SEGMENT_SIZE,
                                        8, color_set[j]);

                        /* draw wheels */
                        basic_fill_rect(x,
                                        details.y + 7, 3, 3,
                                        wheel_color);
                        basic_fill_rect(x + 4,
                                        details.y + 7, 3, 3,
                                        wheel_color);
                        basic_fill_rect(x + 8,
                                        details.y + 7, 3, 3,
                                        wheel_color);
                }
        }
        return EASTER_EGG_SEGMENT_SIZE;
#else
        for (i = 0; i < bar_width; i += 1)
                basic_fill_rect(
                    title_width + 3 + details.x + i * LOADING_SEGMENT_SIZE,
                    details.y, LOADING_SEGMENT_SIZE, LOADING_SEGMENT_SIZE,
                    color_set[(i + options->offset) % color_set_size]);
#endif

        return LOADING_SEGMENT_SIZE;
}