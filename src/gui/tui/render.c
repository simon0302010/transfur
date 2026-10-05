#include "render.h"
#include "../../misc/console.h"
#include "widgets/text.h"

void tui_render(struct ui_context *ctx) {
        int i, width;

        /* Works on Windows, MS-DOS and POSIX systems */
        width = get_console_size().columns;

        for (i = 0; i < ctx->count; i++) {
                tui_render_widget(ctx->widgets[i], width);
        }
}

void tui_render_widget(struct widget widget, int width) {
        switch (widget.type) {
        case WIDGET_TEXT:
                tui_render_text_widget(widget, width);
                break;
        case WIDGET_LOADING_BAR:
                tui_render_loading_bar_widget(widget, width);
                break;
        case WIDGET_PROGRESS_BAR:
                tui_render_progress_bar_widget(widget, width);
                break;
        case WIDGET_TEXT_INPUT:
                tui_render_text_input_widget(widget, width);
                break;
        case WIDGET_BUTTON:
                tui_render_button_widget(widget, width);
                break;
        default:
                break;
        }
}