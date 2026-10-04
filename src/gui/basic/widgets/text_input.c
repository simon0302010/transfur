#include "../../core/widgets/text_input.h"
#include "../sdl3/basic_wrapper.h"
#include "text_input.h"
#include "../text.h"

#define INPUT_PADDING 4
#define INPUT_BORDER_SIZE 2

static struct color text_color = {255, 255, 255};
static struct color border_color = {255, 255, 255};
static struct color focus_color = {255, 255, 0};

int basic_render_text_input_widget(struct widget widget,
                                   struct basic_render_details details) {
        struct widget_text_input_options *options = widget.content;
        int text_height;

        /* top border */
        basic_fill_rect(details.x, details.y, details.width, INPUT_BORDER_SIZE,
                        border_color);

        text_height = draw_text(options->buffer,
                                details.x + INPUT_BORDER_SIZE + INPUT_PADDING,
                                details.y + INPUT_BORDER_SIZE + INPUT_PADDING,
                                2, text_color)
                          .height;

        /* left & right borders */
        basic_fill_rect(details.x, details.y, INPUT_BORDER_SIZE,
                        INPUT_BORDER_SIZE + INPUT_PADDING + text_height +
                            INPUT_PADDING,
                        border_color);
        basic_fill_rect(details.x + details.width - INPUT_BORDER_SIZE,
                        details.y, INPUT_BORDER_SIZE,
                        INPUT_BORDER_SIZE + INPUT_PADDING + text_height +
                            INPUT_PADDING,
                        border_color);

        /* bottom border */
        basic_fill_rect(details.x,
                        details.y + INPUT_BORDER_SIZE + INPUT_PADDING +
                            text_height + INPUT_PADDING,
                        details.width, INPUT_BORDER_SIZE, border_color);

        if (options->is_focused) {
                basic_draw_rect(details.x + INPUT_BORDER_SIZE,
                                details.y + INPUT_BORDER_SIZE,
                                details.width - INPUT_BORDER_SIZE -
                                    INPUT_BORDER_SIZE,
                                INPUT_BORDER_SIZE + text_height, focus_color);
        }
}