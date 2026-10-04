#include "sdl3/basic_wrapper.h"

struct text_metrics {
        int width;
        int height;
};

struct text_metrics draw_text(char *text, int x, int y, int size, struct color color);