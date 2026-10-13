#include "render.h"
#include "../../misc/console.h"
#include "widgets/button.h"
#include "widgets/loading_bar.h"
#include "widgets/text.h"
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>

void tui_render(struct ui_context *ctx) {
        int i, width;

        /* Works on Windows, MS-DOS and POSIX systems */
        width = get_console_size().columns;

        for (i = 0; i < ctx->count; i++) {
                char **lines = tui_render_widget(ctx->widgets[i], width);
                char *line;
                size_t j = 0;

                while ((line = lines[j++]) != NULL) {
                        printf("%s", line);
                        free(line);
                }

                free(lines);
        }
}

char **tui_render_widget(struct widget widget, int width) {
        switch (widget.type) {
        case WIDGET_TEXT:
                return tui_render_text_widget(widget, width);
                break;
        case WIDGET_LOADING_BAR:
                return tui_render_loading_bar_widget(widget, width);
                break;
        case WIDGET_PROGRESS_BAR:
                return tui_render_progress_bar_widget(widget, width);
                break;
        case WIDGET_TEXT_INPUT:
                return tui_render_text_input_widget(widget, width);
                break;
        case WIDGET_BUTTON:
                return tui_render_button_widget(widget, width);
                break;
        case WIDGET_SPLIT:
                return tui_render_split_widget(widget, width);
                break;
        case WIDGET_GROUP:
                return tui_render_group_widget(widget, width);
                break;
        default:
                break;
        }
}