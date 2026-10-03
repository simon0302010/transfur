#ifndef TUI_RENDER_H
#define TUI_RENDER_H

#include "../core/ui_core.h"

struct basic_render_details {
        int x;
        int y;
        int width;
};

void basic_render(struct ui_context *ctx);
int basic_render_widget(struct widget widget,
                         struct basic_render_details details);

#endif /* TUI_RENDER_H */