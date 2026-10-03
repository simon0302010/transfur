#include "../../core/widget.h"
#include <stdio.h>

void tui_render_text_widget(struct widget widget) {
        printf("%s\n", (char *)widget.content);
        fflush(stdout);
}
