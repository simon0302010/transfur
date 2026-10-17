#include "loading_bar.h"
#include "../../core/widgets/loading_bar.h"
#include "../ansi.h"
#include <malloc.h>
#include <string.h>

char **tui_render_loading_bar_widget(struct widget widget, int width) {
        struct widget_loading_bar_options *options = widget.content;

        short i;
        short charset_size = 6;
        const char charset[6] = {'%', '$', '&', '-', '-', '-'};
        size_t title_width = strlen(options->title);
        int bar_width = width - (int)title_width - 3;
        char *output_text;
        char **output;

        if (bar_width <= 0) {
                char **output = {NULL};
                return output;
        }

        output_text = malloc((size_t)width + 1);

        if (!output_text) {
                char **output = {NULL};
                return output;
        }

        strcpy(output_text, options->title);

        /* seperator between title and bar */
        output_text[title_width] = ' ';
        output_text[title_width + 1] = '|';
        output_text[title_width + 2] = ' ';

        for (i = 0; i < bar_width; i++)
                output_text[i + title_width + 3] =
                    charset[(i + options->offset) % charset_size];

        output_text[bar_width + title_width + 3] = '\0';

        /* TODO: re-enable ANSI (temporarily disabled for base dynamic loading
        bar rendering imlementation) #ifdef USE_ANSI
                printf("%s|%s%s%s|%s%s%s|%s\n", ANSI_COLOR_CYAN,
        ANSI_COLOR_YELLOW, options->title, ANSI_COLOR_CYAN, ANSI_COLOR_GREEN,
        loading_bar, ANSI_COLOR_CYAN, ANSI_COLOR_RESET); #else
                printf("|%s|%s|\n", options->title, loading_bar);
        #endif
        */

        output = malloc(sizeof(char *) * 2);
        if (!output) {
                free(output_text);
                return NULL;
        }
        output[0] = output_text;
        output[1] = NULL;
        return output;
}