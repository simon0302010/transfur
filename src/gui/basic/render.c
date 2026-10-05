#include "render.h"
#include "widgets/text.h"

#define BASIC_MARGIN 8
#define BASIC_SPACING 4

void basic_render(struct ui_context *ctx) {
        int i;
        int height = BASIC_MARGIN;
        struct basic_render_details details;

        details.width = 800 - BASIC_MARGIN - BASIC_MARGIN; /* TODO: Match window */
        details.x = BASIC_MARGIN;

        for (i = 0; i < ctx->count; i++) {
                details.y = height;
                height += basic_render_widget(ctx->widgets[i], details) + BASIC_SPACING;
        }
}

int basic_render_widget(struct widget widget,
                        struct basic_render_details details) {
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
        case WIDGET_TEXT_INPUT:
                return basic_render_text_input_widget(widget, details);
                break;
        case WIDGET_BUTTON:
                return basic_render_button_widget(widget, details);
                break;
        default:
                break;
        }
}