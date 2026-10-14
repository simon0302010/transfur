#include "../text.h"
#include "../../core/widget.h"
#include <malloc.h>

char **tui_render_text_widget(struct widget widget, int width) {
        return wrap_text(widget.content, width);
}
