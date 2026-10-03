#include "loading_bar.h"
#include "../../core/widgets/loading_bar.h"
#include "../ansi.h"
#include <malloc.h>
#include <string.h>

void tui_render_loading_bar_widget(struct widget widget, int width) {
        struct widget_loading_bar_options *options = widget.content;

        short i;
        short charset_size = 6;
        const char charset[6] = {'%', '$', '&', '-', '-', '-'};
        size_t title_width = strlen(options->title);
        int bar_width = width - (int)title_width - 3;
        char *loading_bar;

        if (bar_width <= 0)
                return;

        loading_bar = malloc((size_t)bar_width + 1);
        if (!loading_bar)
                return;

        for (i = 0; i < bar_width; i++)
                loading_bar[i] = charset[(i + options->offset) % charset_size];

        loading_bar[bar_width] = '\0';

#ifdef USE_ANSI
        printf("%s|%s%s%s|%s%s%s|%s\n", ANSI_COLOR_CYAN, ANSI_COLOR_YELLOW,
               options->title, ANSI_COLOR_CYAN, ANSI_COLOR_GREEN, loading_bar,
               ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
#else
        printf("|%s|%s|\n", options->title, loading_bar);
#endif
        fflush(stdout);

        free(loading_bar);
}