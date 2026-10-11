#include "../../core/widgets/button.h"
#include "../design.h"
#include "../images.h"
#include "../sdl3/basic_wrapper.h"
#include "../text.h"
#include "../theme.h"
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

        if (button_width <= 0)
                return 0;

        title_height = measure_text(options->title, 1).height;

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
                            button_width,
                            title_height + BORDER_WIDTH + BORDER_WIDTH +
                                BUTTON_MARGIN + BUTTON_MARGIN,
                            BORDER_ROUNDED, focus_color);
                else
                        basic_fill_rounded_rect(
                            details.x + PADDING, details.y + PADDING,
                            button_width,
                            title_height + BORDER_WIDTH + BORDER_WIDTH +
                                BUTTON_MARGIN + BUTTON_MARGIN,
                            BORDER_ROUNDED, border_color);

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
                draw_image_scaled(IMAGE_TEST_FONT,
                                  details.x + BORDER_WIDTH + PADDING,
                                  details.y + BORDER_WIDTH + PADDING,
                                  button_width - BORDER_WIDTH - BORDER_WIDTH,
                                  title_height + BUTTON_MARGIN + BUTTON_MARGIN);

                draw_text(
                    options->title, details.x + BORDER_WIDTH + BUTTON_MARGIN,
                    details.y + BORDER_WIDTH + BUTTON_MARGIN, 1, text_color);

                return title_height + BORDER_WIDTH + BORDER_WIDTH +
                       BUTTON_MARGIN + BUTTON_MARGIN + PADDING + PADDING;
#endif
        }
}