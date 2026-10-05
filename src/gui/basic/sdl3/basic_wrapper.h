#ifndef BASIC_WRAPPER_H
#define BASIC_WRAPPER_H

#include "../../../misc/bool.h"

struct color {
        int r;
        int g;
        int b;
};

enum basic_event_type {
        BASIC_EVENT_NONE,
        BASIC_EVENT_QUIT,
        BASIC_EVENT_MOUSE_DOWN,
        BASIC_EVENT_MOUSE_UP,
        BASIC_EVENT_MOUSE_MOVE,
        BASIC_EVENT_KEY_DOWN
};

enum basic_key {
        BASIC_KEY_NONE,
        BASIC_KEY_ESC,
        BASIC_KEY_TAB,
        BASIC_KEY_SHIFT_TAB,
        BASIC_KEY_ENTER,
        BASIC_KEY_BACKSPACE,
        BASIC_KEY_DELETE,
        BASIC_KEY_UP,
        BASIC_KEY_DOWN,
        BASIC_KEY_LEFT,
        BASIC_KEY_RIGHT
};

struct basic_event {
        enum basic_event_type type;
        int mouse_x;
        int mouse_y;
        int mouse_button;
        enum basic_key key;
        char ch;
};

int basic_init(const char *title, int width, int height);

void basic_cleanup(void);

void basic_begin_frame(void);

void basic_present(void);

tbool basic_poll_event(struct basic_event *event);

double basic_get_time(void);

void basic_clear(struct color color);

void basic_set_pixel(int x, int y, struct color color);

void basic_draw_rect(int x, int y, int w, int h, struct color color);

void basic_fill_rect(int x, int y, int w, int h, struct color color);

#endif