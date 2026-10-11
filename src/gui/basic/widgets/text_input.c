#include "../../core/widgets/text_input.h"
#include "../design.h"
#include "../sdl3/basic_wrapper.h"
#include "../text.h"
#include "text_input.h"

int basic_render_text_input_widget(struct widget widget,
                                   struct basic_render_details details) {
        struct widget_text_input_options *options = widget.content;
        int text_height;

        int input_width = details.width - PADDING - PADDING;

        if (input_width <= 0)
                return 0;

        text_height = measure_text(options->buffer, 2).height;

        if (widget.is_focused)
                basic_fill_rounded_rect(
                    details.x + PADDING, details.y + PADDING, input_width,
                    text_height + BORDER_WIDTH + BORDER_WIDTH + BUTTON_MARGIN +
                        BUTTON_MARGIN,
                    BORDER_ROUNDED, focus_color);
        else
                basic_fill_rounded_rect(
                    details.x + PADDING, details.y + PADDING, input_width,
                    text_height + BORDER_WIDTH + BORDER_WIDTH + BUTTON_MARGIN +
                        BUTTON_MARGIN,
                    BORDER_ROUNDED, border_color);

        basic_fill_rounded_rect(details.x + BORDER_WIDTH + PADDING,
                                details.y + BORDER_WIDTH + PADDING,
                                input_width - BORDER_WIDTH - BORDER_WIDTH,
                                text_height + BUTTON_MARGIN + BUTTON_MARGIN,
                                BORDER_ROUNDED, primary_button_color);

        draw_text(options->buffer, details.x + BORDER_WIDTH + BUTTON_MARGIN,
                  details.y + BORDER_WIDTH + BUTTON_MARGIN, 1, text_color);

        return text_height + BORDER_WIDTH + BORDER_WIDTH + BUTTON_MARGIN +
               BUTTON_MARGIN + PADDING + PADDING;
}