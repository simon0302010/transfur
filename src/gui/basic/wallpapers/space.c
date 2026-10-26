#include "../../../misc/math.h"
#include "../theme.h"
#include "../wallpaper.h"
#include "colors.h"
#include <math.h>
#include <stddef.h>

#define SPACE_MIN_STARS 50
#define SPACE_MAX_STARS 100

#define SPACE_MIN_FLAMES 5
#define SPACE_MAX_FLAMES 40

void basic_draw_space_wallpaper(struct color *wallpaper_pixels,
                                unsigned int width, unsigned int height) {

        unsigned int num_flames = basic_wallpaper_get_random() *
                                      (SPACE_MAX_FLAMES - SPACE_MIN_FLAMES) +
                                  SPACE_MIN_FLAMES;

        unsigned int num_stars =
            basic_wallpaper_get_random() * (SPACE_MAX_STARS - SPACE_MIN_STARS) +
            SPACE_MIN_STARS;

        unsigned int i;
        int x, y;

        for (y = 0; y < height; y++) {
                for (x = 0; x < width; x++) {
                        wallpaper_pixels[y * width + x].r = 0;
                        wallpaper_pixels[y * width + x].g = 0;
                        wallpaper_pixels[y * width + x].b = 0;
                }
        }

        for (i = 0; i < num_stars; i++) {
                struct color pixel_color = {255, 255, 255};
                x = basic_wallpaper_get_random() * width;
                y = basic_wallpaper_get_random() * height;

                wallpaper_pixels[y * width + x] = pixel_color;

                while (basic_wallpaper_get_random() < 0.8) {
                        x += (int)(basic_wallpaper_get_random() * 3) - 1;
                        y += (int)(basic_wallpaper_get_random() * 3) - 1;
                        if (x < 0 || x >= width || y < 0 || y >= height) {
                                break;
                        }
                        if (wallpaper_pixels[y * width + x].r != 0)
                                continue;

                        pixel_color.r = (unsigned char)(pixel_color.r * 0.8f);
                        pixel_color.g = (unsigned char)(pixel_color.g * 0.8f);
                        pixel_color.b = (unsigned char)(pixel_color.b * 0.8f);

                        wallpaper_pixels[y * width + x] = pixel_color;
                }
        }

        for (i = 0; i < num_flames; i++) {
                struct color pixel_color = {210, 0, 0};
                int dx = (int)(basic_wallpaper_get_random() * 3) - 1;
                int dy = (int)(basic_wallpaper_get_random() * 3) - 1;
                int og_x, og_y;
                if (dx == 0 && dy == 0) {
                        dx = 1;
                }
                x = basic_wallpaper_get_random() * width;
                y = basic_wallpaper_get_random() * height;
                og_x = x;
                og_y = y;

                wallpaper_pixels[y * width + x] = pixel_color;

                while (basic_wallpaper_get_random() < 0.95) {
                        if (basic_wallpaper_get_random() < 0.5) {
                                x += dx;
                                y += dy;
                        } else {
                                x +=
                                    (int)(basic_wallpaper_get_random() * 3) - 1;
                                y +=
                                    (int)(basic_wallpaper_get_random() * 3) - 1;
                        }

                        if (x < 0 || x >= width || y < 0 || y >= height) {
                                break;
                        }

                        pixel_color.g += 5;
                        if (pixel_color.g > 255) {
                                pixel_color.g = 255;
                        }

                        wallpaper_pixels[y * width + x] = pixel_color;
                }

                /* second burn */
                x = og_x;
                y = og_y;

                while (basic_wallpaper_get_random() < 0.8) {
                        struct color existing_color;
                        if (basic_wallpaper_get_random() < 0.5) {
                                x += dx;
                                y += dy;
                        } else {
                                x +=
                                    (int)(basic_wallpaper_get_random() * 3) - 1;
                                y +=
                                    (int)(basic_wallpaper_get_random() * 3) - 1;
                        }

                        if (x < 0 || x >= width || y < 0 || y >= height) {
                                break;
                        }

                        existing_color = wallpaper_pixels[y * width + x];

                        existing_color.r *= 1.25;

                        if (existing_color.r > 255) {
                                existing_color.r = 255;
                        }

                        existing_color.g *= 0.95;

                        if (existing_color.g < 0) {
                                existing_color.g = 0;
                        }

                        wallpaper_pixels[y * width + x] = existing_color;
                }
        }
}