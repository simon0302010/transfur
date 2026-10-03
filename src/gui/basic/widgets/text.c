#include "../text.h"
#include "../../core/widget.h"

void render_text_widget(struct widget widget) {
        draw_text(widget.content, 0, 0, 2); /* TODO: Account for y position */
}
