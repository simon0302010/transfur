#include <stddef.h>

enum widget_type {
        WIDGET_TEXT,
        WIDGET_PROGRESS_BAR,
        WIDGET_LOADING_BAR,
        WIDGET_TEXT_INPUT,
        WIDGET_GROUP /* this will replace BASIC_PANEL */
};

struct widget {
        enum widget_type type;
        size_t content_size;
        void *content;
};

struct widget widget_text(int length, const char *content);
struct widget widget_progress_bar(struct progress_bar_options *opts);
struct widget widget_loading_bar(struct loading_bar_options *opts);
struct widget widget_text_input(struct text_input_options *opts);
struct widget widget_group(struct widget *children, size_t count);
