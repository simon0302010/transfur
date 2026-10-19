#include "../text.h"
#include "../../core/widget.h"

char **tui_render_text_widget(struct widget widget, int width) {
        return wrap_text(widget.content, width);
}
