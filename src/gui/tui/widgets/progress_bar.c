#include "progress_bar.h"
#include "../../core/widgets/progress_bar.h"
#include "../ansi.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

void tui_render_progress_bar_widget(struct widget widget, int width) {
        /*
        TODO: Add alignment of options and multiple renderables
        in a row.
        */

        struct widget_progress_bar_options *options = widget.content;
        int heading_chars;
        int progress_chars;
        int remaining_width;
        char progress_string[8];
        /* Turns XX% into a string */
        sprintf(progress_string, "%i%%", (int)(*(options->progress) * 100));

        /* Calculate the widths now that we have progress_string
         */
        heading_chars = strlen(options->title) + strlen(progress_string) + 4;
        progress_chars = (*(options->progress)) * (width - heading_chars);
        remaining_width = width - heading_chars - progress_chars;

#ifdef USE_ANSI
        printf("%s|%s%s%s|%s%s%s|%s", ANSI_COLOR_CYAN, ANSI_COLOR_YELLOW,
               options->title, ANSI_COLOR_CYAN, ANSI_COLOR_MAGENTA,
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
}
