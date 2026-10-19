#include "../../core/widget.h"
#include "../render.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char **tui_render_split_widget(struct widget widget, int width) {
        int i;
        int sep_total;
        int avail_width;
        struct widget *children;
        char ***outputs;
        char **output;
        size_t *lines;
        size_t max_lines = 0;
        size_t width_per_child;

        if (widget.content_size <= 0) {
                output = malloc(sizeof(char *));
                output[0] = NULL;
                return output;
        }

        children = widget.content;
        sep_total = (widget.content_size - 1) * 3;
        avail_width = width - sep_total;
        if (avail_width < (int)widget.content_size) {
                avail_width = (int)widget.content_size;
        }
        width_per_child = avail_width / widget.content_size;
        if (width_per_child == 0) {
                width_per_child = 1;
        }

        outputs = malloc(sizeof(char **) * widget.content_size);
        lines = malloc(sizeof(size_t) * widget.content_size);

        for (i = 0; i < widget.content_size; i++) {
                size_t j = 0;
                size_t child_lines = 0;

                outputs[i] =
                    tui_render_widget(children[i], (int)width_per_child);

                if (outputs[i] != NULL) {
                        while (outputs[i][j++] != NULL) {
                                child_lines++;
                        }
                }
                lines[i] = child_lines;
                if (child_lines > max_lines) {
                        max_lines = child_lines;
                }
        }

        output = malloc(sizeof(char *) * (max_lines + 1));

        for (i = 0; i < max_lines; i++) {
                size_t j;
                size_t pos = 0;
                size_t line_length = 0;

                for (j = 0; j < widget.content_size; j++) {
                        size_t child_len = (outputs[j] != NULL && i < lines[j])
                                               ? strlen(outputs[j][i])
                                               : 0;
                        size_t col_width = (child_len > width_per_child)
                                               ? child_len
                                               : width_per_child;
                        line_length += col_width + 3; /* " | " */
                }

                output[i] = malloc(line_length + 1);

                for (j = 0; j < widget.content_size; j++) {
                        size_t child_len = 0;
                        if (outputs[j] != NULL && i < lines[j]) {
                                child_len = strlen(outputs[j][i]);
                                memcpy(output[i] + pos, outputs[j][i],
                                       child_len);
                                pos += child_len;
                        }
                        while (child_len < width_per_child) {
                                output[i][pos++] = ' ';
                                child_len++;
                        }
                        if (j < widget.content_size - 1) {
                                memcpy(output[i] + pos, " | ", 3);
                                pos += 3;
                        }
                }
                output[i][pos] = '\0';
        }

        for (i = 0; i < widget.content_size; i++) {
                if (outputs[i] != NULL) {
                        size_t j = 0;
                        while (outputs[i][j] != NULL) {
                                free(outputs[i][j]);
                                j++;
                        }
                        free(outputs[i]);
                }
        }
        free(outputs);
        free(lines);

        output[max_lines] = NULL;
        return output;
}
