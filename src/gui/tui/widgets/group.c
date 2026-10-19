#include "../../core/widget.h"
#include "../render.h"
#include <stddef.h>
#include <stdlib.h>

char **tui_render_group_widget(struct widget widget, int width) {
        int i;
        struct widget *children = widget.content;
        char ***outputs;
        size_t total_lines = 0;
        size_t current_line = 0;
        char **output;

        if (widget.content_size <= 0) {
                output = malloc(sizeof(char *));
                output[0] = NULL;
                return output;
        }

        outputs = malloc(sizeof(char **) * widget.content_size);

        for (i = 0; i < widget.content_size; i++) {
                size_t j = 0;

                outputs[i] = tui_render_widget(children[i], width);

                if (outputs[i] != NULL) {
                        while (outputs[i][j++] != NULL) {
                                total_lines++;
                        }
                }
        }

        output = malloc(sizeof(char *) * (total_lines + 1));

        for (i = 0; i < widget.content_size; i++) {
                if (outputs[i] != NULL) {
                        size_t j = 0;

                        while (outputs[i][j] != NULL) {
                                output[current_line++] = outputs[i][j];
                                j++;
                        }
                        free(outputs[i]);
                }
        }
        free(outputs);

        output[current_line] = NULL;
        return output;
}
