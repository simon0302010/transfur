#include "sdl3/basic_wrapper.h"
#include <math.h>

void basic_draw_line(int x1, int y1, int x2, int y2, struct color color);

void basic_draw_arc(double cx, double cy, double r, double start, double end,
                    struct color color);
void basic_fill_arc(double cx, double cy, double r, double start, double end,
                    struct color color);

void basic_draw_circle(double cx, double cy, double r, struct color color);
void basic_fill_circle(double cx, double cy, double r, struct color color);