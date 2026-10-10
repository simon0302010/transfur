#include "../../core/widgets/button.h"
#include "../design.h"
#include "../sdl3/basic_wrapper.h"
#include "../text.h"
#include "button.h"

#ifdef ENABLE_EASTER_EGGS
#include <math.h>
#endif

int basic_render_button_widget(struct widget widget,
                               struct basic_render_details details) {
        struct widget_button_options *options = widget.content;

        short i;
        short color_set_size = 6;

        int button_width = details.width - 3;
        int title_height;

        if (button_width <= 0)
                return 0;

        title_height = draw_text(options->title, details.x + BORDER_WIDTH,
                                 details.y + BORDER_WIDTH, 1, text_color)
                           .height;

        if (widget.is_focused)
                basic_fill_rounded_rect(details.x, details.y, button_width,
                                        title_height + BORDER_WIDTH +
                                            BORDER_WIDTH + BUTTON_MARGIN +
                                            BUTTON_MARGIN,
                                        BORDER_ROUNDED, focus_color);
        else
                basic_fill_rounded_rect(details.x, details.y, button_width,
                                        title_height + BORDER_WIDTH +
                                            BORDER_WIDTH + BUTTON_MARGIN +
                                            BUTTON_MARGIN,
                                        BORDER_ROUNDED, border_color);
        basic_fill_rounded_rect(details.x + BORDER_WIDTH,
                                details.y + BORDER_WIDTH,
                                button_width - BORDER_WIDTH - BORDER_WIDTH,
                                title_height + BUTTON_MARGIN + BUTTON_MARGIN,
                                BORDER_ROUNDED, button_color);

#ifdef ENABLE_EASTER_EGGS
        for (i = 0; i < button_width - BORDER_WIDTH - BORDER_WIDTH; i++) {
                struct color color;
                color.r = (int)(100.0 * ((sin(i % 9) + 2.0) / 2.0));
                color.g = (int)(40.0 * ((cos(i / 9) + 2.0) / 2.0));
                color.b = (int)(40.0 * ((tan(i * 9) + 2.0) / 2.0));

                basic_fill_rect(details.x + BORDER_WIDTH + i,
                                details.y + BORDER_WIDTH, 1, title_height,
                                color);
        }
#endif

        draw_text(options->title, details.x + BORDER_WIDTH + BUTTON_MARGIN,
                  details.y + BORDER_WIDTH + BUTTON_MARGIN, 1, text_color);

        return title_height + BORDER_WIDTH + BORDER_WIDTH;
}