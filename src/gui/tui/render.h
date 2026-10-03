#ifndef TUI_RENDER_H
#define TUI_RENDER_H

#include "../core/ui_core.h"

void tui_render(struct ui_context *ctx);
void tui_render_widget(struct widget widget, int width);

#endif /* TUI_RENDER_H */