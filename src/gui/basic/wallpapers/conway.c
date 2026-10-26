#include "conway.h"
#include "../wallpaper.h"
#include "../wrappers/basic_wrapper.h"
#include <stdlib.h>

#define CONWAY_MIN_CYCLES 20
#define CONWAY_MAX_CYCLES 80

#define CONWAY_MIN_SCALE 2
#define CONWAY_MAX_SCALE 8

#define CONWAY_MIN_STARTERS 12
#define CONWAY_MAX_STARTERS 48

#define CONWAY_STARTER_SIZE 4

#define MAX_COLORS 12

struct color conway_colors[] = {{255, 0, 0},   {0, 255, 0},    {0, 0, 255},
                                {225, 225, 0}, {255, 45, 128}, {220, 20, 95},
                                {255, 95, 31}, {255, 200, 0},  {34, 215, 122},
                                {16, 168, 92}, {162, 230, 65}, {0, 210, 255}};

void basic_draw_conway_wallpaper(struct color *wallpaper_pixels,
                                 unsigned int width, unsigned int height) {
        unsigned int scale = basic_wallpaper_get_random() *
                                 (CONWAY_MAX_SCALE - CONWAY_MIN_SCALE) +
                             CONWAY_MIN_SCALE;
        unsigned int num_starters =
            basic_wallpaper_get_random() *
                (CONWAY_MAX_STARTERS - CONWAY_MIN_STARTERS) +
            CONWAY_MIN_STARTERS;

        unsigned int scale_width = width / scale;
        unsigned int scale_height = height / scale;

        unsigned int i, y, x, sy, sx;

        int num_cycles = basic_wallpaper_get_random() *
                             (CONWAY_MAX_CYCLES - CONWAY_MIN_CYCLES) +
                         CONWAY_MIN_CYCLES;

        /* true = alive */
        tbool *prev_field =
            (tbool *)calloc(scale_width * scale_height, sizeof(tbool));

        tbool *next_field =
            (tbool *)calloc(scale_width * scale_height, sizeof(tbool));

        struct color color =
            conway_colors[(int)(basic_wallpaper_get_random() * MAX_COLORS)];

        /* add starters */
        for (i = 0; i < num_starters; i++) {
                unsigned int x = basic_wallpaper_get_random() *
                                 (scale_width - 1 - CONWAY_STARTER_SIZE);
                unsigned int y = basic_wallpaper_get_random() *
                                 (scale_height - 1 - CONWAY_STARTER_SIZE);
                for (sy = 0; sy < CONWAY_STARTER_SIZE; sy++) {
                        for (sx = 0; sx < CONWAY_STARTER_SIZE; sx++) {
                                prev_field[(y + sy) * scale_width + (x + sx)] =
                                    basic_wallpaper_get_random() < 0.5f;
                        }
                }
        }

        /* reset wallpaper_pixels to black */
        for (sy = 0; sy < height; sy++) {
                for (sx = 0; sx < width; sx++) {
                        wallpaper_pixels[sy * width + sx].r = 0;
                        wallpaper_pixels[sy * width + sx].b = 0;
                        wallpaper_pixels[sy * width + sx].g = 0;
                }
        }

        for (i = 0; i < num_cycles; i++) {
                for (y = 0; y < scale_height; y++) {
                        for (x = 0; x < scale_width; x++) {
                                unsigned int state =
                                    prev_field[y * scale_width + x];
                                unsigned int neighbors = 0;
                                int neighbor_x, neighbor_y;
                                for (neighbor_y = -1; neighbor_y <= 1;
                                     neighbor_y++) {
                                        for (neighbor_x = -1; neighbor_x <= 1;
                                             neighbor_x++) {
                                                int check_x, check_y;
                                                if (neighbor_x == 0 &&
                                                    neighbor_y == 0) {
                                                        continue;
                                                }
                                                check_x = x + neighbor_x;
                                                check_y = y + neighbor_y;
                                                if (check_x < 0 ||
                                                    check_x >=
                                                        (int)scale_width ||
                                                    check_y < 0 ||
                                                    check_y >=
                                                        (int)scale_height) {
                                                        continue;
                                                }
                                                if (prev_field[check_y *
                                                                   scale_width +
                                                               check_x]) {
                                                        neighbors++;
                                                }
                                        }
                                }

                                if (state) {
                                        if (neighbors < 2 ||
                                            neighbors > 3) { /* underpopulation
                                                                / overpopulation
                                                              */
                                                next_field[y * scale_width +
                                                           x] = false;
                                        } else { /* survival */
                                                next_field[y * scale_width +
                                                           x] = true;
                                        }
                                } else {
                                        if (neighbors == 3) { /* reproduction */
                                                next_field[y * scale_width +
                                                           x] = true;
                                        } else {
                                                next_field[y * scale_width +
                                                           x] = false;
                                        }
                                }
                        }
                }

                /* draw conway */
                if (i % 5 == 0) {
                        for (y = 0; y < scale_height; y++) {
                                for (x = 0; x < scale_width; x++) {
                                        struct color existing_color =
                                            wallpaper_pixels[(y * scale) *
                                                                 width +
                                                             (x * scale)];
                                        struct color new_color;
                                        unsigned int value =
                                            next_field[(y)*scale_width + (x)];
                                        if (value) {
                                                new_color = color;
                                        } else {
                                                new_color.r = 0;
                                                new_color.g = 0;
                                                new_color.b = 0;
                                        }
                                        if (new_color.r == existing_color.r &&
                                            new_color.g == existing_color.g &&
                                            new_color.b == existing_color.b) {
                                                continue;
                                        }
                                        new_color.r =
                                            (existing_color.r + new_color.r) /
                                            2;
                                        new_color.g =
                                            (existing_color.g + new_color.g) /
                                            2;
                                        new_color.b =
                                            (existing_color.b + new_color.b) /
                                            2;
                                        for (sy = 0; sy < scale; sy++) {
                                                for (sx = 0; sx < scale; sx++) {
                                                        if (y + sy < height &&
                                                            x + sx < width) {
                                                                wallpaper_pixels
                                                                    [(y * scale +
                                                                      sy) *
                                                                         width +
                                                                     (x * scale +
                                                                      sx)] =
                                                                        new_color;
                                                        }
                                                }
                                        }
                                }
                        }
                }

                free(prev_field);
                prev_field = next_field;
                next_field =
                    (tbool *)calloc(scale_width * scale_height, sizeof(tbool));
                color = conway_colors[(int)(basic_wallpaper_get_random() *
                                            MAX_COLORS)];
        }

        free(prev_field);
        free(next_field);
}