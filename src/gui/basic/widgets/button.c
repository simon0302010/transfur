#include "../../core/widgets/button.h"
#include "../design.h"
#include "../images.h"
#include "../text.h"
#include "../theme.h"
#include "../wrappers/basic_wrapper.h"
#include "button.h"

#ifdef ENABLE_EASTER_EGGS
#include <math.h>
#endif

int basic_render_button_widget(struct widget widget,
                               struct basic_render_details details) {
        struct widget_button_options *options = widget.content;

        short i;
        short color_set_size = 6;

        int button_width = details.width - 3 - PADDING - PADDING;
        int title_height;

        struct color button_color;

        enum widget_button_styles style;

        int h;

        if (button_width <= 0)
                return 0;

        title_height = measure_text(options->title, 1).height;

        h = title_height + BORDER_WIDTH + BORDER_WIDTH + BUTTON_MARGIN +
            BUTTON_MARGIN;

        if (options->style != NULL)
                style = *options->style;
        else
                style = WIDGET_BUTTON_STYLE_DISABLED;

        switch (basic_get_theme()) {
        case THEME_CLASSIC:
        default:

                if (widget.is_focused)
                        basic_fill_rounded_rect(
                            details.x + PADDING, details.y + PADDING,
                            button_width, h, BORDER_ROUNDED, focus_color);
                else
                        basic_fill_rounded_rect(
                            details.x + PADDING, details.y + PADDING,
                            button_width, h, BORDER_ROUNDED, border_color);

                switch (style) {
                case WIDGET_BUTTON_STYLE_PRIMARY:
                default:
                        button_color = primary_button_color;
                        break;
                case WIDGET_BUTTON_STYLE_SUCCESS:
                        button_color = success_button_color;
                        break;
                case WIDGET_BUTTON_STYLE_FAILURE:
                        button_color = failure_button_color;
                        break;
                case WIDGET_BUTTON_STYLE_DISABLED:
                        button_color = disabled_button_color;
                        break;
                }

                basic_fill_rounded_rect(
                    details.x + BORDER_WIDTH + PADDING,
                    details.y + BORDER_WIDTH + PADDING,
                    button_width - BORDER_WIDTH - BORDER_WIDTH,
                    title_height + BUTTON_MARGIN + BUTTON_MARGIN,
                    BORDER_ROUNDED, button_color);
                draw_text(
                    options->title, details.x + BORDER_WIDTH + BUTTON_MARGIN,
                    details.y + BORDER_WIDTH + BUTTON_MARGIN, 1, text_color);

                return title_height + BORDER_WIDTH + BORDER_WIDTH +
                       BUTTON_MARGIN + BUTTON_MARGIN + PADDING + PADDING;
                break;
#ifdef ENABLE_THEMES
        case THEME_COFFEE:
        case THEME_PASTEL: {
                basic_image_id_t image;
                int image_height;

                switch (style) {
                case WIDGET_BUTTON_STYLE_PRIMARY:
                default:
                        image = (basic_get_theme() == THEME_COFFEE)
                                    ? IMAGE_COFFEE_BUTTON_PRIMARY
                                    : IMAGE_PASTEL_BUTTON_PRIMARY;
                        break;
                case WIDGET_BUTTON_STYLE_SUCCESS:
                        image = (basic_get_theme() == THEME_COFFEE)
                                    ? IMAGE_COFFEE_BUTTON_SUCCESS
                                    : IMAGE_PASTEL_BUTTON_SUCCESS;
                        break;
                case WIDGET_BUTTON_STYLE_FAILURE:
                        image = (basic_get_theme() == THEME_COFFEE)
                                    ? IMAGE_COFFEE_BUTTON_FAILURE
                                    : IMAGE_PASTEL_BUTTON_FAILURE;
                        break;
                case WIDGET_BUTTON_STYLE_DISABLED:
                        image = (basic_get_theme() == THEME_COFFEE)
                                    ? IMAGE_COFFEE_BUTTON_DISABLED
                                    : IMAGE_PASTEL_BUTTON_DISABLED;
                        break;
                }

                draw_image_tiled(image, details.x + BORDER_WIDTH + PADDING,
                                 details.y + BORDER_WIDTH + PADDING,
                                 button_width - BORDER_WIDTH - BORDER_WIDTH,
                                 title_height + BUTTON_MARGIN + BUTTON_MARGIN,
                                 title_height + BUTTON_MARGIN + BUTTON_MARGIN,
                                 title_height + BUTTON_MARGIN + BUTTON_MARGIN);

                draw_image_tiled((basic_get_theme() == THEME_COFFEE)
                                     ? IMAGE_COFFEE_BORDERS_HORIZONTAL
                                     : IMAGE_PASTEL_BORDERS_HORIZONTAL,
                                 details.x + PADDING, details.y + PADDING,
                                 details.width - PADDING - PADDING,
                                 title_height + BORDER_WIDTH + BORDER_WIDTH +
                                     BUTTON_MARGIN + BUTTON_MARGIN,
                                 title_height + BUTTON_MARGIN + BUTTON_MARGIN +
                                     BORDER_WIDTH + BORDER_WIDTH,
                                 title_height + BUTTON_MARGIN + BUTTON_MARGIN +
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

                draw_text(
                    options->title, details.x + BORDER_WIDTH + BUTTON_MARGIN,
                    details.y + BORDER_WIDTH + BUTTON_MARGIN, 1,
                    (basic_get_theme() == THEME_COFFEE) ? text_color
                                                        : text_color_inverted);

                return title_height + BORDER_WIDTH + BORDER_WIDTH +
                       BUTTON_MARGIN + BUTTON_MARGIN + PADDING + PADDING;
        }
#endif
        }
}