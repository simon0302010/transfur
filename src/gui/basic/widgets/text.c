#include "../text.h"
#include "../../core/widget.h"
#include "../design.h"
#include "../theme.h"
#include "text.h"

int basic_render_text_widget(struct widget widget,
                             struct basic_render_details details) {
        return draw_text(
                   widget.content, details.x, details.y, TEXT_SIZE_REGULAR,
                   (basic_get_theme() != THEME_PASTEL) ? text_color
                                                       : text_color_inverted)
            .height;
}

int basic_measure_text_widget(struct widget widget,
                              struct basic_render_details details) {
        return measure_text(widget.content, TEXT_SIZE_REGULAR).height;
}
