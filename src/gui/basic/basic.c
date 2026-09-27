#include "basic.h"
#include "../basic_wrapper/basic_wrapper.h"
#include "text.h"
#include <stdbool.h>

void run_basic(char *title, int width, int height, struct basic_renderable renderables[], size_t count) {
        bool running = true;
        struct color color;

        color.r = 255;
        color.b = 0;
        color.g = 0;
        
        basic_init(title, width, height);

        while (running) {
                basic_begin_frame();

                basic_fill_rect(12, 14, 16, 18, color);

                draw_text("Hello world! abcdefghijklmnopqrstuvwxyz 0123456789", 34, 34, 3, color);

                basic_present();
        }

        return;
}
