#include "tui.h"
#include <stddef.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

/* #define WIDTH_OVERRIDE 0 */

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
                                printf("%s\n", renderables[i].content);
                                break;
                        case RENDERABLE_GROUP:
                                render_content(renderables[i].content, renderables[i].content_size);
                                break;
                        case PROGRESS_BAR: {
                                struct progress_bar_options *options = renderables[i].content;
                                /*
                                        title + pipes (3) + spaces (3) + XXX% (4)
                                        TODO: handle XX% or X%
                                */
                                int heading_chars = renderables[i].content_size - sizeof(int) + 3+3+4;
                                int progress_chars = (*(options->progress)) * (width - heading_chars);
                                int remaining_width = width - heading_chars - progress_chars;
                                
                                printf("%s | %d%% |", options->title, (int)(*(options->progress) * 100));

                                for (; progress_chars > 0; progress_chars--) {
                                        printf("%%");
                                }

                                for (; remaining_width > 0; remaining_width--) {
                                        printf("-");
                                }

                                printf("|\n");

                                break;
                        }
                }
        }
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

struct renderable create_progress_bar(struct progress_bar_options *options, size_t size) {
        struct renderable new_progress_bar;

        new_progress_bar.type = PROGRESS_BAR;

        new_progress_bar.content_size = size;

        new_progress_bar.content = options;

        return new_progress_bar;
}
