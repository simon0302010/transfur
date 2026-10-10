#include "drawing.h"
#include "../../misc/math.h"
#include "sdl3/basic_wrapper.h"

void basic_draw_line(int x1, int y1, int x2, int y2, struct color color) {
        double slope = (double)(y2 - y1) / (double)(x2 - x1);
        double slope_distance = sqrt(slope * slope + 1.0);
        double total_distance =
            sqrt(pow((double)(x2 - x1), 2) + pow((double)(y2 - y1), 2));
        int x;
        int first_x;
        int final_x;

        if (x1 < x2) {
                first_x = x1;
                final_x = x2;
        } else {
                first_x = x2;
                final_x = x1;
        }

        for (x = first_x; x < final_x; x++) {
                int y = (slope * (double)x + 0.5);

                basic_set_pixel(x, y, color);
        }
}

void basic_draw_circle(int cx, int cy, int r, struct color color) {
        double radians_per_step = 1.0 / (double)r;
        double angle;

        for (angle = 0.0; angle < (2.0 * M_PI); angle += radians_per_step) {
                basic_set_pixel(cx + (int)(r * cos(angle) + 0.5),
                                cy + (int)(r * sin(angle) + 0.5), color);
        }
}

void basic_fill_circle(int cx, int cy, int r, struct color color) {
        double radians_per_step = 1.0 / (double)r;
        double angle;

        for (angle = 0.0; angle < (2.0 * M_PI); angle += radians_per_step) {
                basic_draw_line(cx, cy, cx + (int)(r * cos(angle) + 0.5),
                                cy + (int)(r * sin(angle) + 0.5), color);
        }
}
