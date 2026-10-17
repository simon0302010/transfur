#include "text.h"
#include <malloc.h>
#include <stddef.h>
#include <string.h>

char **wrap_text(char *text, size_t max_width) {
        size_t lines;
        char **output;
        char *line;
        size_t cursor = 0;
        size_t line_i = 0;
        size_t output_i = 0;
        char ch;

        if (max_width == 0) {
                max_width = 1;
        }

        lines = (strlen(text) + max_width - 1) / max_width + 1; /* +1 for NULL */
        output = malloc(lines * sizeof(char *));
        line = malloc(max_width + 1);

        while ((ch = text[cursor++]) != '\0') {
                line[line_i++] = ch;
                if (line_i >= max_width) {
                        line[line_i] = '\0';
                        line_i = 0;
                        output[output_i++] = line;
                        line = malloc(max_width + 1);
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
