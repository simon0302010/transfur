#include "../../core/widget.h"
#include <malloc.h>

char **tui_render_text_widget(struct widget widget, int width) {
        size_t lines;
        char **output;
        size_t cursor = 0;
        char ch;
        size_t line_i = 0;
        size_t output_i = 0;
        char *line;

        if (width <= 0 || !widget.content)
                return NULL;

        lines = (widget.content_size + width - 1) / width;

        output = malloc((lines + 1) * sizeof(char *));

        line = malloc(width + 1);

        while ((ch = ((char *)widget.content)[cursor++]) != '\0') {
                line[line_i++] = ch;
                if (line_i >= width) {
                        line[line_i] = '\0';
                        output[output_i++] = line;
                        line = malloc(width + 1);
                        line_i = 0;
                }
        }

        if (line_i > 0) {
                line[line_i] = '\0';
                output[output_i++] = line;
        } else {
                free(line);
        }

        output[output_i] = NULL;

        return output;
}
