#include "../../core/widgets/button.h"
#include "../sdl3/basic_wrapper.h"
#include "../text.h"
#include "button.h"

#define BUTTON_PADDING 2

#define ENABLE_EASTER_EGGS

#ifdef ENABLE_EASTER_EGGS
#include <math.h>
#endif

static struct color text_color = {255, 255, 255};
static struct color bg_color = {20, 20, 40};
static struct color border_color = {40, 40, 60};

int basic_render_button_widget(struct widget widget,
                               struct basic_render_details details) {
        struct widget_button_options *options = widget.content;

        short i;
        short color_set_size = 6;

        int bar_width = details.width - 3;
        int title_height;

        if (bar_width <= 0)
                return 0;

        title_height = draw_text(options->title, details.x + BUTTON_PADDING,
                                 details.y + BUTTON_PADDING, 1, text_color)
                           .height;

        basic_fill_rect(details.x, details.y, bar_width,
                        title_height + BUTTON_PADDING + BUTTON_PADDING,
                        border_color);
        basic_fill_rect(details.x + BUTTON_PADDING, details.y + BUTTON_PADDING,
                        bar_width - BUTTON_PADDING - BUTTON_PADDING,
                        title_height, bg_color);

        #ifdef ENABLE_EASTER_EGGS      
                for (i=0; i < bar_width - BUTTON_PADDING - BUTTON_PADDING; i++) {
                        struct color color;
                        color.r = (int)(100.0 * ((sin(i % 9)+2.0)/2.0));
                        color.g = (int)(40.0 * ((cos(i / 9)+2.0)/2.0));
                        color.b = (int)(40.0 * ((tan(i * 9)+2.0)/2.0));

                        basic_fill_rect(details.x + BUTTON_PADDING + i, details.y + BUTTON_PADDING, 1, title_height, color);
                }
        #endif
        
        draw_text(options->title, details.x + BUTTON_PADDING,
                  details.y + BUTTON_PADDING, 1, text_color);

        

        return title_height + BUTTON_PADDING + BUTTON_PADDING;
}