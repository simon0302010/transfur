#include "../text.h"
#include "../../core/widget.h"

struct color default_text_color = {255, 255, 255}; /* White */

void basic_render_text_widget(struct widget widget) {
        draw_text(widget.content, 0, 0, 2,
                  default_text_color); /* TODO: Account for y position */
}
