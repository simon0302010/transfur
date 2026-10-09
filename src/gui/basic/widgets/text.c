#include "../text.h"
#include "../../core/widget.h"
#include "../design.h"
#include "text.h"

int basic_render_text_widget(struct widget widget,
                             struct basic_render_details details) {
        return draw_text(widget.content, details.x, details.y,
                         TEXT_SIZE_REGULAR, text_color)
            .height;
}
