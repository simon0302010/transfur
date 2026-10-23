#include "../../core/widgets/button.h"
#include "../text.h"
#include "button.h"
#include <../ansi.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char **tui_render_button_widget(struct widget widget, int width) {
        struct widget_button_options *options = widget.content;
        int length = strlen(options->title);
#ifdef USE_ANSI
        char *text = malloc(length + 4 + 1 + strlen(ANSI_BG_COLOR_BLUE) +
                            strlen(ANSI_RESET));
#else
        char *text = malloc(length + 4 + 1);
#endif
        char **output;
        unsigned int i = 0;

/* TODO: add ANSI support (if available) for button styles */
#ifdef USE_ANSI
        enum widget_button_styles style;
        if (options->style != NULL)
                style = *options->style;
        else
                style = WIDGET_BUTTON_STYLE_DISABLED;

        switch (style) {
        case WIDGET_BUTTON_STYLE_PRIMARY:
        default:
                strcpy(text, ANSI_BG_COLOR_BLUE);
                i += strlen(ANSI_BG_COLOR_BLUE);
                break;
        case WIDGET_BUTTON_STYLE_SUCCESS:
                strcpy(text, ANSI_BG_COLOR_GREEN);
                i += strlen(ANSI_BG_COLOR_GREEN);
                break;
        case WIDGET_BUTTON_STYLE_FAILURE:
                strcpy(text, ANSI_BG_COLOR_RED);
                i += strlen(ANSI_BG_COLOR_RED);
                break;
        case WIDGET_BUTTON_STYLE_DISABLED:
                strcpy(text, ANSI_BG_COLOR_YELLOW);
                i += strlen(ANSI_BG_COLOR_YELLOW);
                break;
        }
#endif

        /* add starting brackets & arrows if focuses */
        if (widget.is_focused) {
                text[i++] = '>';
                text[i++] = '{';
                strcpy(text + i, options->title);
                i += length;
                text[i++] = '}';
                text[i++] = '<';
                text[i++] = '\0';
        } else {
                text[i++] = '{';
                text[i++] = ' ';
                strcpy(text + i, options->title);
                i += length;
                text[i++] = ' ';
                text[i++] = '}';
                text[i++] = '\0';
        }

#ifdef USE_ANSI
        strcat(text, ANSI_RESET);
#endif

        output = wrap_text(text, width);
        free(text);
        return output;
}
