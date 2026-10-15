#include "progress_bar.h"
#include "../../core/widgets/progress_bar.h"
#include "../design.h"
#include "../images.h"
#include "../theme.h"
#include "../wrappers/basic_wrapper.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define BAR_HEIGHT 8

static struct color bar_color = {0, 20, 0};
static struct color progress_color = {0, 255, 0};

int basic_render_progress_bar_widget(struct widget widget,
                                     struct basic_render_details details) {
        /*
        TODO: Add alignment of options and multiple renderables
        in a row.
        */

        struct widget_progress_bar_options *options = widget.content;
        int heading_width;
        int progress_chars;
        int remaining_width;
        char progress_string[50];
        /* Turns XX% into a string */
        sprintf(progress_string, "%s|%i%%", options->title,
                (int)(*(options->progress) * 100));

        /* Calculate the widths now that we have progress_string
         */

        heading_width = draw_text(progress_string, details.x, details.y, 1,
                                  (basic_get_theme() != THEME_PASTEL)
                                      ? text_color
                                      : text_color_inverted);
        remaining_width = details.width - heading_width;

        switch (basic_get_theme()) {
        case THEME_CLASSIC:
        default:

                basic_fill_rect(details.x + heading_width, details.y,
                                remaining_width, BAR_HEIGHT, bar_color);
                basic_fill_rect(
                    details.x + heading_width, details.y,
                    (int)((double)remaining_width * *options->progress),
                    BAR_HEIGHT, progress_color);
                break;
#ifdef ENABLE_THEMES
        case THEME_COFFEE:
        case THEME_PASTEL:
                draw_image_tiled((basic_get_theme() == THEME_COFFEE)
                                     ? IMAGE_COFFEE_PROGRESS_BAR_BACKGROUND
                                     : IMAGE_PASTEL_PROGRESS_BAR_BACKGROUND,
                                 details.x + heading_width, details.y,
                                 remaining_width, BAR_HEIGHT, BAR_HEIGHT,
                                 BAR_HEIGHT);
                draw_image_tiled(
                    (basic_get_theme() == THEME_COFFEE)
                        ? IMAGE_COFFEE_PROGRESS_BAR_SEGMENT
                        : IMAGE_COFFEE_PROGRESS_BAR_SEGMENT,
                    details.x + heading_width, details.y,
                    (int)((double)remaining_width * *options->progress),
                    BAR_HEIGHT, BAR_HEIGHT, BAR_HEIGHT);
                draw_image_scaled_cropped(
                    (basic_get_theme() == THEME_COFFEE)
                        ? IMAGE_COFFEE_PROGRESS_BAR_END
                        : IMAGE_COFFEE_PROGRESS_BAR_END,
                    details.x + heading_width +
                        (int)((double)remaining_width * *options->progress),
                    details.y, BAR_HEIGHT, BAR_HEIGHT, 0, 0,
                    details.x + details.width, details.y + BAR_HEIGHT);
                break;
#endif
        }

        return BAR_HEIGHT;
}
