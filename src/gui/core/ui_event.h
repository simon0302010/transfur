#ifndef CORE_UI_EVENT_H
#define CORE_UI_EVENT_H

enum ui_key {
        UI_KEY_NONE,
        UI_KEY_CHAR,
        UI_KEY_ENTER,
        UI_KEY_BACKSPACE,
        UI_KEY_DELETE,
        UI_KEY_TAB,
        UI_KEY_SHIFT_TAB,
        UI_KEY_ESC,
        UI_KEY_LEFT,
        UI_KEY_RIGHT,
        UI_KEY_UP,
        UI_KEY_DOWN,
        UI_KEY_MOUSE_BUTTON_ONE,
        UI_KEY_MOUSE_BUTTON_TWO,
        UI_KEY_MOUSE_BUTTON_THREE,
        UI_KEY_MOUSE_BUTTON_FOUR,
        UI_KEY_MOUSE_BUTTON_FIVE
};

enum ui_event_type {
        UI_EVENT_QUIT,
        UI_EVENT_KEY_DOWN,
        UI_EVENT_KEY_UP,
        UI_EVENT_MOUSE_DOWN,
        UI_EVENT_MOUSE_UP,
        UI_EVENT_MOUSE_MOVE
};

struct ui_event {
        enum ui_event_type type;
        enum ui_key key;
        char ch;
        int mouse_x; /* always 0 in TUI environment */
        int mouse_y;
};

#endif /* CORE_UI_EVENT_H */