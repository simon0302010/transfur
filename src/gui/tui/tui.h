#include <stddef.h>

enum renderable_type {
        RENDERABLE_GROUP,
        TEXT,
        PROGRESS_BAR,
        TEXT_INPUT
};

struct renderable {
        enum renderable_type type;
        size_t content_size;
        void *content;
};

struct progress_bar_options {
        float *progress;
        char *title;
};

struct text_input_options {
        const char *label;
        char *buffer;
        size_t max_len;
        size_t cursor;
        int is_focused;
};

void render_content(struct renderable renderable[], size_t count);


struct renderable create_text(int length, char *content);

struct renderable create_group(struct renderable *children, size_t count);

struct renderable create_progress_bar(struct progress_bar_options *options, size_t size);

struct renderable create_text_input(struct text_input_options *options, size_t size);
