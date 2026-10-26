#include "../../core/widgets/text_input.h"
#include "../ansi.h"
#include "../text.h"
#include "loading_bar.h"
#include <stdlib.h>
#include <string.h>

char **tui_render_text_input_widget(struct widget widget, int width) {
        struct widget_text_input_options *options = widget.content;
        size_t label_size = strlen(options->label);
        size_t buffer_size = strlen(options->buffer);
        char *text;
        char **output;
#ifdef USE_ANSI
        size_t i = 0;
        size_t cursor;
#endif

#ifdef USE_ANSI
        /* all the below + BG + reset */
        text = malloc(label_size + 1 + 1 + options->max_len + 3 +
                      sizeof(ANSI_BG_COLOR_CYAN) + sizeof(ANSI_RESET));
#else
        /* label + space + '[' + buffer + cursor/bracket + null terminator */
        text = malloc(label_size + 1 + 1 + options->max_len + 3);
#endif
        if (!text) {
                return NULL;
        }

#ifdef USE_ANSI
        memcpy(text + i, options->label, label_size);
        i += label_size;
        text[i++] = ' ';
        text[i++] = '[';

        if (widget.is_focused) {
                cursor = options->cursor;
                if (cursor > buffer_size) {
                        cursor = buffer_size;
                }

                /* 1. Copy text before cursor */
                memcpy(text + i, options->buffer, cursor);
                i += cursor;

                /* 2. Highlight cursor */
                memcpy(text + i, ANSI_BG_COLOR_CYAN,
                       sizeof(ANSI_BG_COLOR_CYAN) - 1);
                i += sizeof(ANSI_BG_COLOR_CYAN) - 1;

                if (cursor < buffer_size) {
                        text[i++] = options->buffer[cursor];
                } else {
                        text[i++] = '_';
                }

                memcpy(text + i, ANSI_RESET, sizeof(ANSI_RESET) - 1);
                i += sizeof(ANSI_RESET) - 1;

                /* 3. Copy text after cursor */
                if (cursor < buffer_size) {
                        size_t remaining = buffer_size - (cursor + 1);
                        memcpy(text + i, options->buffer + cursor + 1, remaining);
                        i += remaining;
                }
        } else {
                memcpy(text + i, options->buffer, buffer_size);
                i += buffer_size;
        }

        text[i++] = ']';
        text[i] = '\0';
#else
        strcpy(text, options->label);
        text[label_size] = ' ';
        text[label_size + 1] = '[';
        strcpy(text + label_size + 2, options->buffer);

        if (widget.is_focused) {
                text[label_size + 2 + buffer_size] = ' ';
                text[label_size + 2 + options->cursor] = '_';
                text[label_size + 3 + buffer_size] = ']';
                text[label_size + 4 + buffer_size] = '\0';
        } else {
                text[label_size + 2 + buffer_size] = ']';
                text[label_size + 3 + buffer_size] = '\0';
        }
#endif

        output = wrap_text(text, width);
        free(text);
        return output;
}
