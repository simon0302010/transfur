#include "drawing.h"
#include "../../misc/math.h"
#include "sdl3/basic_wrapper.h"
#include <math.h>
#include <stdlib.h>

void basic_draw_line(int x1, int y1, int x2, int y2, struct color color) {
        double slope = (double)(y2 - y1) / (double)(x2 - x1);
        double slope_distance = sqrt(slope * slope + 1.0);
        double total_distance =
            sqrt(pow((double)(x2 - x1), 2) + pow((double)(y2 - y1), 2));
        int i;
        int step = 1;
        int dx = abs(x2 - x1);

        if (x2 < x1) {
                step = -1;
        }

        for (i = 0; i <= dx; i++) {
                int x = i * step;
                int y = slope * (x) + 0.5;

                basic_set_pixel(x1 + x, y1 + y, color);
        }
}

void basic_draw_arc(double cx, double cy, double r, double start, double end,
                    struct color color) {
        double radians_per_step = 1.0 / (double)r;
        double angle;

        for (angle = start; angle < end; angle += radians_per_step) {
                basic_set_pixel((int)(cx + r * cos(angle) + 0.5),
                                (int)(cy + r * sin(angle) + 0.5), color);
        }
}

void basic_fill_arc(double cx, double cy, double r, double start, double end,
                    struct color color) {
        double radians_per_step = 1.0 / (double)r;
        double angle;

        for (angle = start; angle < end; angle += radians_per_step) {
                basic_draw_line((int)cx, (int)cy,
                                (int)(cx + r * cos(angle) + 0.5),
                                (int)(cy + r * sin(angle) + 0.5), color);
        }
}

void basic_draw_circle(double cx, double cy, double r, struct color color) {
        basic_draw_arc(cx, cy, r, 0.0, 2.0 * M_PI, color);
}

void basic_fill_circle(double cx, double cy, double r, struct color color) {
        basic_fill_arc(cx, cy, r, 0.0, 2.0 * M_PI, color);
}
