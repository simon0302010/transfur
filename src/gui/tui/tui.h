#ifndef TUI_H
#define TUI_H

#include <stddef.h>
#include "raw.h"

enum renderable_type { RENDERABLE_GROUP, TEXT, PROGRESS_BAR, TEXT_INPUT, LOADING_BAR };

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

struct text_input_options {
        const char *label;
        char *buffer;
        size_t max_len;
        size_t cursor;
        int is_focused;
};

void render_content(struct renderable renderables[], size_t count);

void run_tui(struct renderable renderables[], size_t count);

struct renderable create_text(int length, char *content);

struct renderable create_group(struct renderable *children, size_t count);

struct renderable create_progress_bar(struct progress_bar_options *options,
                                      size_t size);

struct renderable create_loading_bar(struct loading_bar_options *options,
                                     size_t size);

struct renderable create_text_input(struct text_input_options *options, size_t size);

#endif