#include "../../core/widget.h"
#include "../render.h"
#include <malloc.h>
#include <stddef.h>

char **tui_render_group_widget(struct widget widget, int width) {
        int i;

        struct widget *children = widget.content;

        char ***outputs = malloc(sizeof(char **) * widget.content_size);

        size_t total_lines = 0;

        char **output;

        for (i = 0; i < widget.content_size; i++) {
                size_t j = 0;

                outputs[i] = tui_render_widget(children[i], width);

                while (outputs[i][j++] != NULL) {
                        total_lines++;
                }
        }

        output = malloc(sizeof(char *) * (total_lines + 1));

        for (i = 0; i < widget.content_size; i++) {
                size_t j = 0;

                while (outputs[i][j] != NULL) {
                        output[j] = outputs[i][j];
                        j++;
                }
        }

        output[total_lines] = NULL;
        return output;
}
