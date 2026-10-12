#include "../../core/widgets/text_input.h"
#include "../design.h"
#include "../images.h"
#include "../sdl3/basic_wrapper.h"
#include "../text.h"
#include "../theme.h"
#include "text_input.h"

int basic_render_text_input_widget(struct widget widget,
                                   struct basic_render_details details) {
        struct widget_text_input_options *options = widget.content;
        int text_height;

        int input_width = details.width - PADDING - PADDING;

        int h;

        if (input_width <= 0)
                return 0;

        text_height = measure_text(options->buffer, 2).height;

        h = text_height + BORDER_WIDTH + BORDER_WIDTH + BUTTON_MARGIN +
            BUTTON_MARGIN;

        switch (basic_get_theme()) {
        case THEME_CLASSIC:
        default:

                if (widget.is_focused)
                        basic_fill_rounded_rect(
                            details.x + PADDING, details.y + PADDING,
                            input_width,
                            text_height + BORDER_WIDTH + BORDER_WIDTH +
                                BUTTON_MARGIN + BUTTON_MARGIN,
                            BORDER_ROUNDED, focus_color);
                else
                        basic_fill_rounded_rect(
                            details.x + PADDING, details.y + PADDING,
                            input_width,
                            text_height + BORDER_WIDTH + BORDER_WIDTH +
                                BUTTON_MARGIN + BUTTON_MARGIN,
                            BORDER_ROUNDED, border_color);

                basic_fill_rounded_rect(
                    details.x + BORDER_WIDTH + PADDING,
                    details.y + BORDER_WIDTH + PADDING,
                    input_width - BORDER_WIDTH - BORDER_WIDTH,
                    text_height + BUTTON_MARGIN + BUTTON_MARGIN, BORDER_ROUNDED,
                    primary_button_color);
                break;
#ifdef ENABLE_THEMES
        case THEME_COFFEE:
        case THEME_PASTEL:
                draw_image_tiled((basic_get_theme() == THEME_COFFEE)
                                     ? IMAGE_COFFEE_INPUT
                                     : IMAGE_PASTEL_INPUT,
                                 details.x + BORDER_WIDTH + PADDING,
                                 details.y + BORDER_WIDTH + PADDING,
                                 input_width - BORDER_WIDTH - BORDER_WIDTH,
                                 text_height + BUTTON_MARGIN + BUTTON_MARGIN,
                                 text_height + BUTTON_MARGIN + BUTTON_MARGIN,
                                 text_height + BUTTON_MARGIN + BUTTON_MARGIN);

                draw_image_tiled((basic_get_theme() == THEME_COFFEE)
                                     ? IMAGE_COFFEE_BORDERS_HORIZONTAL
                                     : IMAGE_PASTEL_BORDERS_HORIZONTAL,
                                 details.x + PADDING, details.y + PADDING,
                                 details.width - PADDING - PADDING,
                                 text_height + BORDER_WIDTH + BORDER_WIDTH +
                                     BUTTON_MARGIN + BUTTON_MARGIN,
                                 text_height + BUTTON_MARGIN + BUTTON_MARGIN +
                                     BORDER_WIDTH + BORDER_WIDTH,
                                 text_height + BUTTON_MARGIN + BUTTON_MARGIN +
                                     BORDER_WIDTH + BORDER_WIDTH);

                /* left */
                draw_image_scaled_cropped(
                    (basic_get_theme() == THEME_COFFEE)
                        ? IMAGE_COFFEE_BORDERS_VERTICAL
                        : IMAGE_PASTEL_BORDERS_VERTICAL,
                    details.x + PADDING, details.y + PADDING, h, h,
                    details.x + PADDING, details.y + PADDING,
                    details.x + PADDING + h / 2, details.y + PADDING + h);

                /* right */
                draw_image_scaled_cropped(
                    (basic_get_theme() == THEME_COFFEE)
                        ? IMAGE_COFFEE_BORDERS_VERTICAL
                        : IMAGE_PASTEL_BORDERS_VERTICAL,
                    details.x + details.width - PADDING - h,
                    details.y + PADDING, h, h,
                    details.x + details.width - PADDING - h / 2,
                    details.y + PADDING, details.x + details.width - PADDING,
                    details.y + PADDING + h);
                break;
#endif
        }

        draw_text(options->buffer, details.x + BORDER_WIDTH + BUTTON_MARGIN,
                  details.y + BORDER_WIDTH + BUTTON_MARGIN, 1,
                  (basic_get_theme() != THEME_PASTEL) ? text_color
                                                      : text_color_inverted);

        return text_height + BORDER_WIDTH + BORDER_WIDTH + BUTTON_MARGIN +
               BUTTON_MARGIN + PADDING + PADDING;
}