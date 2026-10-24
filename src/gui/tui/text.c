#include "text.h"
#include "ansi.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static size_t get_ansi_sequence_length(const char *str) {
#ifdef USE_ANSI
        if (str[0] == '\x1b' && str[1] == '[') {
                size_t i = 2;
                while (str[i] != '\0' && (str[i] < '@' || str[i] > '~')) {
                        i++;
                }
                if (str[i] != '\0') {
                        return i + 1;
                }
        }
#endif
        return 0;
}

size_t get_visible_width(const char *str) {
        size_t width = 0;
        size_t i = 0;

        if (str == NULL) {
                return 0;
        }

        while (str[i] != '\0') {
#ifdef USE_ANSI
                size_t ansi_length = get_ansi_sequence_length(&str[i]);
                if (ansi_length > 0) {
                        i += ansi_length;
                        continue;
                }
#endif
                i++;
                width++;
        }
        return width;
}

char **wrap_text(char *text, size_t max_width) {
        size_t lines_cap = 8;
        char **output;
        char *line;
        size_t cursor = 0;
        size_t output_i = 0;
        char ch;
        size_t line_cap;
        size_t line_length = 0;
        size_t visible_width = 0;

        if (max_width == 0) {
                max_width = 1;
        }

#ifdef USE_ANSI
        line_cap = max_width + 32; /* add room for ANSI sequences */
#else
        line_cap = max_width;
#endif

        output = malloc(lines_cap * sizeof(char *));
        line = malloc(line_cap + 1);

        while ((ch = text[cursor]) != '\0') {

#ifdef USE_ANSI
                size_t ansi_length = get_ansi_sequence_length(&text[cursor]);

                if (ansi_length > 0) {
                        if (line_length + ansi_length + 1 > line_cap) {
                                line_cap = line_length + ansi_length + 32;
                                line = realloc(line, line_cap + 1);
                        }

                        memcpy(line + line_length, &text[cursor], ansi_length);
                        line_length += ansi_length;
                        cursor += ansi_length;
                        continue;
                }
#endif

                cursor++;

                if (line_length + 2 > line_cap) {
                        line_cap *= 2;
                        line = realloc(line, line_cap + 1);
                }

                line[line_length++] = ch;
                visible_width++;

                if (visible_width >= max_width) {
                        line[line_length] = '\0';

                        if (output_i + 2 > lines_cap) {
                                lines_cap *= 2;
                                output =
                                    realloc(output, lines_cap * sizeof(char *));
                        }

                        output[output_i++] = line;

                        line_cap = max_width + 32;
                        line = malloc(line_cap + 1);
                        line_length = 0;
                        visible_width = 0;
                }
        }

        if (line_length > 0) {
                line[line_length] = '\0';

                if (output_i + 2 > lines_cap) {
                        lines_cap *= 2;
                        output = realloc(output, lines_cap * sizeof(char *));
                }

                output[output_i++] = line;
        } else {
                free(line);
        }

        output[output_i] = NULL;

        return output;
}
