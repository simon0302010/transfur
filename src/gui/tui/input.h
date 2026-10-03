#ifndef INPUT_H
#define INPUT_H

#include "../core/ui_event.h"

struct ui_event tui_poll_key_event(int timeout_ms);

#endif