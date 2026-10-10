#include "drawing.h"
#include "../../misc/math.h"
#include "sdl3/basic_wrapper.h"
#include <math.h>
#include <stdlib.h>

void basic_draw_line_low(int x1, int y1, int x2, int y2, struct color color) {
        int dx = x2 - x1;
        int dy = y2 - y1;
        int yi = 1;
        int d;
        int x;
        int y;

        if (dy < 0) {
                yi = -1;
                dy = -dy;
        }

        d = (2 * dy) - dx;
        y = y1;

        for (x = x1; x <= x2; x++) {
                basic_set_pixel(x, y, color);
                if (d > 0) {
                        y = y + yi;
                        d = d + (2 * (dy - dx));
                } else {
                        d = d + 2 * dy;
                }
        }
}

void basic_draw_line_high(int x1, int y1, int x2, int y2, struct color color) {
        int dx = x2 - x1;
        int dy = y2 - y1;
        int xi = 1;
        int d;
        int x;
        int y;

        if (dx < 0) {
                xi = -1;
                dx = -dx;
        }

        d = (2 * dx) - dy;
        x = x1;

        for (y = y1; y <= y2; y++) {
                basic_set_pixel(x, y, color);
                if (d > 0) {
                        x = x + xi;
                        d = d + (2 * (dx - dy));
                } else {
                        d = d + 2 * dx;
                }
        }
}

void basic_draw_line(int x1, int y1, int x2, int y2, struct color color) {
        if (abs(y2 - y1) < abs(x2 - x1)) {
                if (x1 > x2)
                        basic_draw_line_low(x2, y2, x1, y1, color);
                else
                        basic_draw_line_low(x1, y1, x2, y2, color);
        } else {
                if (y1 > y2)
                        basic_draw_line_high(x2, y2, x1, y1, color);
                else
                        basic_draw_line_high(x1, y1, x2, y2, color);
        }
}

void basic_draw_arc(double cx, double cy, double r, double start, double end,
                    struct color color) {
        double radians_per_step = 1.0 / (double)r;
        double angle;

        for (angle = start; angle <= end; angle += radians_per_step) {
                basic_set_pixel((int)(cx + r * cos(angle) + 0.5),
                                (int)(cy + r * sin(angle) + 0.5), color);
        }
}

void basic_fill_arc(double cx, double cy, double r, double start, double end,
                    struct color color) {
        double radians_per_step = 1.0 / (double)r;
        double angle;

        for (angle = start; angle <= end; angle += radians_per_step) {
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

/* Since height is the primary like, measurement in the basic_gui, I am
introducing h_rounding_factor. If this is 60%, then the vertical rect lines will
essentially be: 30% * height = round 40% * height = straight 30% * height =
round (for width, 30%*height will be round on either side, with the remaining
width being straight)
*/
void basic_draw_rounded_rect(int x, int y, int width, int height,
                             double h_rounding_factor, struct color color) {
        int box_size = (int)(h_rounding_factor * height) / 2.0;
        double r = box_size;
        int straight_w = width - box_size - box_size;
        int straight_h = height - box_size - box_size;

        /* top left corner */
        basic_draw_arc(x + r, y + r, r, M_PI, 1.5 * M_PI, color);
        /* top right corner */
        basic_draw_arc(x + width - 1 - r, y + r, r, 1.5 * M_PI, 2.0 * M_PI,
                       color);
        /* bottom right corner */
        basic_draw_arc(x + width - 1 - r, y + height - 1 - r, r, 0.0,
                       0.5 * M_PI, color);
        /* bottom left corner */
        basic_draw_arc(x + r, y + height - 1 - r, r, 0.5 * M_PI, M_PI, color);

        /* straight parts */
        basic_draw_line(x + box_size, y, x + box_size + straight_w, y, color);
        basic_draw_line(x + box_size, y + height - 1, x + box_size + straight_w,
                        y + height - 1, color);
        basic_draw_line(x, y + box_size, x, y + box_size + straight_h, color);
        basic_draw_line(x + width - 1, y + box_size, x + width - 1,
                        y + box_size + straight_h, color);
}

void basic_fill_rounded_rect(int x, int y, int width, int height,
                             double h_rounding_factor, struct color color) {
        int box_size = (int)(h_rounding_factor * height) / 2.0;
        double r = box_size;
        int straight_w = width - box_size - box_size;
        int straight_h = height - box_size - box_size;

        /* middle fill */
        basic_fill_rect(x + box_size, y + box_size, straight_w, straight_h,
                        color);

        /* top left corner */
        basic_fill_arc(x + r, y + r, r, M_PI, 1.5 * M_PI, color);
        /* top right corner */
        basic_fill_arc(x + width - 1 - r, y + r, r, 1.5 * M_PI, 2.0 * M_PI,
                       color);
        /* bottom right corner */
        basic_fill_arc(x + width - 1 - r, y + height - 1 - r, r, 0.0,
                       0.5 * M_PI, color);
        /* bottom left corner */
        basic_fill_arc(x + r, y + height - 1 - r, r, 0.5 * M_PI, M_PI, color);

        /* straight parts */
        basic_fill_rect(x + box_size, y, straight_w, box_size, color);
        basic_fill_rect(x + box_size, y + height - box_size, straight_w,
                        box_size, color);
        basic_fill_rect(x, y + box_size, box_size, straight_h, color);
        basic_fill_rect(x + width - box_size, y + box_size, box_size,
                        straight_h, color);
}
