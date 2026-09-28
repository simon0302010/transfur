#ifndef TUI_H
#define TUI_H

#include "../../misc/bool.h"
#include <stddef.h>

enum tui_key {
        KEY_NONE,
        KEY_CHAR,
        KEY_ENTER,
        KEY_BACKSPACE,
        KEY_TAB,
        KEY_SHIFT_TAB,
        KEY_ESC,
        KEY_ARROW_UP,
        KEY_ARROW_DOWN,
        KEY_ARROW_RIGHT,
        KEY_ARROW_LEFT,
        KEY_DELETE
};

struct tui_event {
        enum tui_key key;
        char ch;
};

enum renderable_type {
        RENDERABLE_GROUP,
        TEXT,
        PROGRESS_BAR,
        TEXT_INPUT,
        LOADING_BAR
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

struct loading_bar_options {
        char *title;
        size_t offset;
        float speed;
        float accumulator;
};

struct text_input_options {
        const char *label;
        char *buffer;
        size_t max_len;
        size_t cursor;
        tbool is_focused;
};

/* when a key event occurs
   we ask the focused widget if it is willing to handle the event
   if this returns false, the widget doesn't care about that event
*/
tbool renderable_handle_event(struct tui_event event,
                              struct renderable renderable);

void update_content(struct renderable renderables[], size_t count, double dt);

void render_content(struct renderable renderables[], size_t count);

void run_tui(struct renderable renderables[], size_t count);

struct renderable create_text(int length, char *content);

struct renderable create_group(struct renderable *children, size_t count);

struct renderable create_progress_bar(struct progress_bar_options *options,
                                      size_t size);

struct renderable create_loading_bar(struct loading_bar_options *options,
                                     size_t size);

struct renderable create_text_input(struct text_input_options *options,
                                    size_t size);

#endif