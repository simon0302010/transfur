#include "progress_bar.h"
#include "../../core/widgets/progress_bar.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **tui_render_progress_bar_widget(struct widget widget, int width) {
        /*
        TODO: Add alignment of options and multiple renderables
        in a row.
        */

        struct widget_progress_bar_options *options = widget.content;
        int heading_chars;
        int progress_chars;
        int remaining_width;
        char progress_string[8];
        char *output_text;
        char **output;
        size_t i;
        /* Turns XX% into a string */
        sprintf(progress_string, "%i%%", (int)(*(options->progress) * 100));

        /* Calculate the widths now that we have progress_string
         */
        heading_chars = strlen(options->title) + strlen(progress_string) + 4;
        if (width <= heading_chars) {
                output = malloc(sizeof(char *));
                if (!output)
                        return NULL;
                output[0] = NULL;
                return output;
        }
        progress_chars = (*(options->progress)) * (width - heading_chars);
        remaining_width = width - heading_chars - progress_chars;
        if (progress_chars < 0) {
                progress_chars = 0;
        }
        if (remaining_width < 0) {
                remaining_width = 0;
        }
        /* TODO: re-enable ANSI (temporarily disabled for base dynamic loading
        #ifdef USE_ANSI
                printf("%s|%s%s%s|%s%s%s|%s", ANSI_COLOR_CYAN,
        ANSI_COLOR_YELLOW, options->title, ANSI_COLOR_CYAN, ANSI_COLOR_MAGENTA,
                       progress_string, ANSI_COLOR_CYAN, ANSI_COLOR_GREEN);
        #else
                printf("|%s|%s|", options->title, progress_string);
        #endif

                for (; progress_chars > 0; progress_chars--) {
                        printf("%%");
                }

        #ifdef USE_ANSI
                printf(ANSI_COLOR_RESET);
        #endif

                for (; remaining_width > 0; remaining_width--) {
                        printf("-");
                }

        #ifdef USE_ANSI
                printf(ANSI_COLOR_CYAN);
        #endif

                printf("|\n");

        #ifdef USE_ANSI
                printf(ANSI_COLOR_RESET);
        #endif

                fflush(stdout);
        */

        output_text = malloc((size_t)width + 1);

        if (!output_text) {
                output = malloc(sizeof(char *));
                if (!output)
                        return NULL;
                output[0] = NULL;
                return output;
        }

        strcpy(output_text, options->title);

        /* seperator between title and bar */
        output_text[strlen(options->title)] = ' ';
        output_text[strlen(options->title) + 1] = '|';
        output_text[strlen(options->title) + 2] = ' ';

        for (i = 0; i < progress_chars; i++)
                output_text[i + strlen(options->title) + 3] = '%';

        for (i = 0; i < remaining_width; i++)
                output_text[i + strlen(options->title) + 3 + progress_chars] =
                    '-';

        output_text[width] = '\0';

        output = malloc(sizeof(char *) * 2);
        if (!output) {
                free(output_text);
                return NULL;
        }
        output[0] = output_text;
        output[1] = NULL;

        return output;
}
