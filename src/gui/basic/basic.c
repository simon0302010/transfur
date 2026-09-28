#include "basic.h"
#include "../basic_wrapper/basic_wrapper.h"
#include "text.h"
#include <stdbool.h>

void run_basic(char *title, int width, int height,
               struct basic_renderable renderables[], size_t count) {
        bool running = true;
        struct color color;
        struct basic_event event;

        color.r = 255;
        color.b = 0;
        color.g = 0;

        basic_init(title, width, height);

        while (running) {
                while (basic_poll_event(&event)) {
                        switch (event.type) {
                        case BASIC_EVENT_QUIT:
                                running = false;
                                break;
                        default:
                                break;
                        }
                }

                basic_begin_frame();

                basic_fill_rect(12, 14, 16, 18, color);

                draw_text("Hello world! abcdefghijklmnopqrstuvwxyz 0123456789",
                          34, 34, 7, color);

                basic_present();
        }

        basic_cleanup();

        return;
}
