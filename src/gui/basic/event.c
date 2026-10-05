#include "event.h"
#include "sdl3/basic_wrapper.h"

struct ui_event basic_poll_ui_event(void) {
        struct basic_event basic_event;
        struct ui_event event;

        basic_poll_event(&basic_event);

        switch (basic_event.type) {
        case BASIC_EVENT_QUIT:
                event.type = UI_EVENT_QUIT;
                break;
        case BASIC_EVENT_KEY_DOWN:
                event.type = UI_EVENT_KEY_DOWN;
                break;
        /* TODO: Add a keyup */
        case BASIC_EVENT_MOUSE_DOWN:
                event.type = UI_EVENT_MOUSE_DOWN;
                break;
        case BASIC_EVENT_MOUSE_UP:
                event.type = UI_EVENT_MOUSE_UP;
                break;
        case BASIC_EVENT_MOUSE_MOVE:
                event.type = UI_EVENT_MOUSE_MOVE;
                break;
        default:
                event.type = UI_EVENT_NONE;
                break;
        }

        event.ch = basic_event.ch;
        event.mouse_x = basic_event.mouse_x;
        event.mouse_y = basic_event.mouse_y;

        if (event.type == UI_EVENT_KEY_DOWN || UI_EVENT_KEY_UP) {
                switch (basic_event.key) {
                case BASIC_KEY_TAB:
                        event.key = UI_KEY_TAB;
                        break;
                case BASIC_KEY_BACKSPACE:
                        event.key = UI_KEY_BACKSPACE;
                        break;
                case BASIC_KEY_ENTER:
                        event.key = UI_KEY_ENTER;
                        break;
                case BASIC_KEY_ESC:
                        event.key = UI_KEY_ESC;
                        break;
                default:
                        event.key = UI_KEY_CHAR;
                        break;
                }
        } else
                event.key = UI_KEY_NONE;

        if (event.key == UI_KEY_CHAR && event.ch == '\0') {
                event.key = UI_KEY_NONE;
        }

        return event;
}