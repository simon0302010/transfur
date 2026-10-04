#include "render.h"
#include "widgets/text.h"


void basic_render(struct ui_context *ctx) {
        int i;
        int height = 0;
        struct basic_render_details details;

        details.width = 800; /* TODO: Match window */
        details.x = 0;

        for (i = 0; i < ctx->count; i++) {
                details.y = height;
                height += basic_render_widget(ctx->widgets[i], details);
        }
}

int basic_render_widget(struct widget widget, struct basic_render_details details) {
        switch (widget.type) {
        case WIDGET_TEXT:
                return basic_render_text_widget(widget, details);
                break;
        case WIDGET_LOADING_BAR:
                return basic_render_loading_bar_widget(widget, details);
                break;
        case WIDGET_PROGRESS_BAR:
                return basic_render_progress_bar_widget(widget, details);
                break;
        /*case WIDGET_TEXT_INPUT:
                tui_render_text_input_widget(widget, width);
                break;*/
        default:
                break;
        }
}