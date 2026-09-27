#ifndef TUI_H
#define TUI_H

#include <stddef.h>

enum renderable_type { RENDERABLE_GROUP, TEXT, PROGRESS_BAR, LOADING_BAR };

struct renderable {
        enum renderable_type type;
        size_t content_size;
        void *content;
};

struct progress_bar_options {
        float *progress;
        char *title;
};

struct loading_bar_options {
        char *title;
        size_t offset;
};

void render_content(struct renderable renderable[], size_t count);

struct renderable create_text(int length, char *content);

struct renderable create_group(struct renderable *children, size_t count);

struct renderable create_progress_bar(struct progress_bar_options *options,
                                      size_t size);

struct renderable create_loading_bar(struct loading_bar_options *options,
                                     size_t size);

#endif