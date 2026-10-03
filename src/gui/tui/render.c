#include "render.h"
#include "../../misc/console.h"
#include "widgets/text.h"

void tui_render(struct ui_context *ctx) {
        int i, width;

        /* Works on Windows, MS-DOS and POSIX systems */
        width = get_console_size().columns;

        for (i = 0; i < ctx->count; i++) {
                switch (ctx->widgets[i].type) {
                case WIDGET_TEXT:
                        tui_render_text_widget(ctx->widgets[i], width);
                        break;
                case WIDGET_LOADING_BAR:
                        tui_render_loading_bar_widget(ctx->widgets[i], width);
                        break;
                default:
                        break;
                }
        }
}
