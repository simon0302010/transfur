#ifndef UI_CORE_H
#define UI_CORE_H

#include "../../misc/bool.h"
#include "ui_event.h"
#include "widget.h"

struct ui_context {
        struct widget *widgets;
        size_t count;
        int focus_index; /* what widget is current focused */
        tbool running;
};

void ui_init(struct ui_context *ctx);
tbool ui_handle_event(struct ui_context *ctx, struct ui_event *event);
void ui_update(struct ui_context *ctx, double dt);

#endif /* UI_CORE_H */