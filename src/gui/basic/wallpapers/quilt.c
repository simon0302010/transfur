#include "quilt.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#define NUM_QUILT_LINES 10
#define NUM_QUILT_LINE_JAGS 25
#define QUILT_RESOLUTION 80

void basic_draw_quilt_wallpaper(struct color *wallpaper_pixels,
                                unsigned int width, unsigned int height) {}

void basic_draw_debug_quilt_wallpaper(struct color *wallpaper_pixels,
                                      unsigned int width, unsigned int height) {
        unsigned int horizontal_spacing =
            width / NUM_QUILT_LINES; /* these lines are vertical, but go along
                                        the horizon */
        unsigned int num_horizontal_lines = height / horizontal_spacing;
        unsigned int vertical_spacing = height / NUM_QUILT_LINES;

        int vertical_lines[QUILT_RESOLUTION][NUM_QUILT_LINES];
        int *horizontal_lines = (int *)malloc(
            QUILT_RESOLUTION * num_horizontal_lines * sizeof(int));

        unsigned int pixels_per_resolution = height / QUILT_RESOLUTION;
        unsigned int resolutions_per_jag =
            QUILT_RESOLUTION / NUM_QUILT_LINE_JAGS;

        size_t i, j, k;

        for (i = 0; i < NUM_QUILT_LINES; i++) {
                int offset = 0;
                int velocity = 0;
                int acceleration = 0;
                for (j = 0; j < QUILT_RESOLUTION; j++) {
                        if (j % resolutions_per_jag == 0) {
                                acceleration =
                                    basic_wallpaper_get_random() * 2 - 1;
                        }
                        velocity += acceleration;
                        offset += velocity;

                        vertical_lines[j][i] = offset;
                }
        }

        pixels_per_resolution = width / QUILT_RESOLUTION;

        for (i = 0; i < num_horizontal_lines; i++) {
                int offset = 0;
                int velocity = 0;
                int acceleration = 0;
                for (j = 0; j < QUILT_RESOLUTION; j++) {
                        if (j % resolutions_per_jag == 0) {
                                acceleration =
                                    basic_wallpaper_get_random() * 2 - 1;
                        }
                        velocity += acceleration;
                        offset += velocity;

                        horizontal_lines[j + i * QUILT_RESOLUTION] = offset;
                }
        }

        /* Debug- draw red vertical lines and blue horizontal lines atop a black
         * background */

        for (i = 0; i < height; i++) {
                for (j = 0; j < width; j++) {
                        wallpaper_pixels[i * width + j].r = 0;
                        wallpaper_pixels[i * width + j].g = 0;
                        wallpaper_pixels[i * width + j].b = 0;
                }
        }

        for (i = 0; i < NUM_QUILT_LINES; i++) {
                unsigned int line_x = i * horizontal_spacing;
                for (j = 0; j < QUILT_RESOLUTION; j++) {
                        int start_y = j * pixels_per_resolution;
                        int end_y = (j + 1) * pixels_per_resolution;
                        int start_offset = vertical_lines[j][i];
                        int end_offset = vertical_lines[j + 1][i];
                        int dy = end_y - start_y;
                        int dx = end_offset - start_offset;

                        for (k = 0; k < pixels_per_resolution; k++) {
                                float progress =
                                    (float)k / (float)pixels_per_resolution;
                                int y = start_y + k;
                                int x = line_x + start_offset + dx * progress;
                                wallpaper_pixels[y * width + x].r = 255;
                                wallpaper_pixels[y * width + x].g = 0;
                                wallpaper_pixels[y * width + x].b = 0;
                        }
                }
        }

        for (i = 0; i < num_horizontal_lines; i++) {
                unsigned int line_y = i * vertical_spacing;
                for (j = 0; j < QUILT_RESOLUTION; j++) {
                        int start_x = j * pixels_per_resolution;
                        int end_x = (j + 1) * pixels_per_resolution;
                        int start_offset = vertical_lines[j][i];
                        int end_offset = vertical_lines[j + 1][i];
                        int dx = end_x - start_x;
                        int dy = end_offset - start_offset;

                        for (k = 0; k < pixels_per_resolution; k++) {
                                float progress =
                                    (float)k / (float)pixels_per_resolution;
                                int x = start_x + k;
                                int y = line_y + start_offset + dy * progress;
                                wallpaper_pixels[y * width + x].r = 0;
                                wallpaper_pixels[y * width + x].g = 0;
                                wallpaper_pixels[y * width + x].b = 255;
                        }
                }
        }
}