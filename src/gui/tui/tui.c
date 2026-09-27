#include "tui.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdbool.h>
#include <poll.h>
#include "raw.h"

/* #define WIDTH_OVERRIDE 0 */
#define USE_ANSI

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

void render_content(struct renderable renderables[], size_t count) {
        int width;
        int i;

#ifdef WIDTH_OVERRIDE
        width = WIDTH_OVERRIDE;
#else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        width = w.ws_col;
#endif

        for (i = 0; i < count; i++) {
                switch (renderables[i].type) {
                case TEXT:
                        printf("%s\n", (char *)renderables[i].content);
                        fflush(stdout);
                        break;
                case RENDERABLE_GROUP:
                        render_content(renderables[i].content,
                                       renderables[i].content_size);
                        break;
                case PROGRESS_BAR: {
                        /*
                        TODO: Add alignment of options and multiple renderables
                        in a row.
                        */

                        struct progress_bar_options *options =
                            renderables[i].content;
                        int heading_chars;
                        int progress_chars;
                        int remaining_width;
                        char progress_string[8];

                        /* Turns XX% into a string */
                        sprintf(progress_string, "%i%%",
                                (int)(*(options->progress) * 100));

                        /* Calculate the widths now that we have progress_string
                         */
                        heading_chars = strlen(options->title) +
                                        strlen(progress_string) + 4;
                        progress_chars =
                            (*(options->progress)) * (width - heading_chars);
                        remaining_width =
                            width - heading_chars - progress_chars;

#ifdef USE_ANSI
                        printf("%s|%s%s%s|%s%s%s|%s", ANSI_COLOR_CYAN,
                               ANSI_COLOR_YELLOW, options->title,
                               ANSI_COLOR_CYAN, ANSI_COLOR_MAGENTA,
                               progress_string, ANSI_COLOR_CYAN,
                               ANSI_COLOR_GREEN);
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

                                break;
                        }

                        fflush(stdout);

                        break;
                case LOADING_BAR: {
                        struct loading_bar_options *options =
                            renderables[i].content;

                        short i;
                        short charset_size = 6;
                        const char charset[6] = {'%', '$', '&', '-', '-', '-'};
                        size_t title_width = strlen(options->title);
                        int bar_width = width - (int)title_width - 3;
                        char *loading_bar;

                        if (bar_width <= 0)
                                break;

                        loading_bar = malloc((size_t)bar_width + 1);
                        if (!loading_bar)
                                break;

                        for (i = 0; i < bar_width; i++)
                                loading_bar[i] = charset[(i + options->offset) %
                                                         charset_size];

                        loading_bar[bar_width] = '\0';

#ifdef USE_ANSI
                        printf("%s|%s%s%s|%s%s%s|%s\n", ANSI_COLOR_CYAN,
                               ANSI_COLOR_YELLOW, options->title,
                               ANSI_COLOR_CYAN, ANSI_COLOR_GREEN, loading_bar,
                               ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
#else
                        printf("|%s|%s|\n", options->title, loading_bar);
#endif
                        fflush(stdout);

                        free(loading_bar);

                        options->offset = (options->offset + 1) % charset_size;

                        break;
                }
                case TEXT_INPUT: {
                                struct text_input_options *options = renderables[i].content;
                                
                                printf("%s [ ... ]", options->label);
                                
                                break;
                }
                }
        }
}

void run_tui(struct renderable renderables[], size_t count) {
        bool running = true;
        int ret;
        char ch;
        struct pollfd pfd;
        
        enable_raw_mode();

        while (running) {
                printf("\x1b[H"); /* TODO: double check this is supported on all systems */
                fflush(stdout);

                render_content(renderables, count);

                ret = poll(&pfd, 1, 50);
                if (ret > 0 && (pfd.revents & POLLIN)) {
                        if (read(STDIN_FILENO, &ch, 1) > 0) {
                                if (ch == 'q' || ch == 27) {
                                        running = false;
                                }
                        }
                }
        }

        disable_raw_mode();
}

struct renderable create_text(int length, char *content) {
        struct renderable new_text;

        new_text.type = TEXT;

        new_text.content_size = length + 1;

        new_text.content = content;

        return new_text;
}

struct renderable create_group(struct renderable *children, size_t count) {
        struct renderable new_group;

        new_group.type = RENDERABLE_GROUP;

        new_group.content_size = count;

        new_group.content = children;

        return new_group;
}

struct renderable create_progress_bar(struct progress_bar_options *options,
                                      size_t size) {
        struct renderable new_progress_bar;

        new_progress_bar.type = PROGRESS_BAR;

        new_progress_bar.content_size = size;

        new_progress_bar.content = options;

        return new_progress_bar;
}


struct renderable create_text_input(struct text_input_options *options, size_t size) {
        struct renderable new_text_input;

        new_text_input.type = TEXT_INPUT;

        new_text_input.content_size = size;

        new_text_input.content = options;

        return new_text_input;
}

struct renderable create_loading_bar(struct loading_bar_options *options,
                                     size_t size) {
        struct renderable new_loading_bar;

        new_loading_bar.type = LOADING_BAR;

        new_loading_bar.content_size = size;

        new_loading_bar.content = options;

        return new_loading_bar;
}