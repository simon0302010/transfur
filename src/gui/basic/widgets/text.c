#include "../text.h"
#include "../../core/widget.h"
#include "text.h"

struct color default_text_color = {255, 255, 255}; /* White */

int basic_render_text_widget(struct widget widget,
                              struct basic_render_details details) {
        return draw_text(widget.content, details.x, details.y, 2, default_text_color).height;
}
