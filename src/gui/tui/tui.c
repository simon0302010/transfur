#include "tui.h"
#include <stddef.h>
#include <stdio.h>

void render_content(struct renderable renderables[], size_t count) {
        int i;
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

                                /* TODO: Make it an actual bar */
                                printf("PROGRESS (%s): %d\n", options->title, *(options->progress));

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
