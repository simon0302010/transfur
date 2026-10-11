#ifndef TUI_RENDER_H
#define TUI_RENDER_H

#include "../core/ui_core.h"

struct basic_render_details {
        int x;
        int y;
        int width;
};

struct basic_hitbox {
        struct widget *widget;
        int x, y, width, height;
};

void basic_render(struct ui_context *ctx);
int basic_render_widget(struct widget *widget,
                        struct basic_render_details details);

static void basic_register_hitbox(struct widget *widget, int x, int y,
                                  int width, int height);

struct widget *basic_get_widget_at(int x, int y);

#endif /* TUI_RENDER_H */