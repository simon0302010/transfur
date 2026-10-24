#include "colors.h"
#include "../theme.h"
#include "../wallpaper.h"
#include <stddef.h>

void basic_draw_colors_wallpaper(struct color *wallpaper_pixels,
                                 unsigned int width, unsigned int height) {
        double brightness = basic_wallpaper_get_random();
        unsigned int r, g, b;
        unsigned int i, j;
        unsigned int tile_size =
            (unsigned int)(basic_wallpaper_get_random() * width * 0.1) + 1;
        unsigned int x, y;

        if (brightness < 0.2)
                brightness = 0.2;

        for (i = 0; i < height / tile_size + 1; i++) {
                for (j = 0; j < width / tile_size + 1; j++) {
                        struct color pixel_color;
                        r = (unsigned int)(basic_wallpaper_get_random() * 255 *
                                           brightness);
                        g = (unsigned int)(basic_wallpaper_get_random() * 255 *
                                           brightness);
                        b = (unsigned int)(basic_wallpaper_get_random() * 255 *
                                           brightness);
                        pixel_color.r = r;
                        pixel_color.g = g;
                        pixel_color.b = b;

                        for (y = 0; y < tile_size; y++) {
                                for (x = 0; x < tile_size; x++) {
                                        unsigned int pixel_x =
                                            j * tile_size + x;
                                        unsigned int pixel_y =
                                            i * tile_size + y;
                                        if (pixel_x < width &&
                                            pixel_y < height) {
                                                wallpaper_pixels[pixel_y *
                                                                     width +
                                                                 pixel_x] =
                                                    pixel_color;
                                        }
                                }
                        }
                }
        }
}

void basic_draw_thematic_colors_wallpaper(struct color *wallpaper_pixels,
                                          unsigned int width,
                                          unsigned int height) {
        double brightness = basic_wallpaper_get_random();
        unsigned int i, j;
        unsigned int tile_size =
            (unsigned int)(basic_wallpaper_get_random() * width * 0.1) + 1;
        unsigned int x, y;

        const struct basic_theme_palette *palette =
            basic_get_theme_palette(basic_get_theme());

        struct color *ptr = (struct color *)palette;

        if (brightness < 0.2)
                brightness = 0.2;

        for (i = 0; i < height / tile_size + 1; i++) {
                for (j = 0; j < width / tile_size + 1; j++) {
                        unsigned int color_index =
                            (unsigned int)(basic_wallpaper_get_random() *
                                           BASIC_THEME_PALETTE_COLORS);
                        struct color pixel_color = ptr[color_index];
                        pixel_color.r =
                            (unsigned int)(pixel_color.r * brightness);
                        pixel_color.g =
                            (unsigned int)(pixel_color.g * brightness);
                        pixel_color.b =
                            (unsigned int)(pixel_color.b * brightness);
                        for (y = 0; y < tile_size; y++) {
                                for (x = 0; x < tile_size; x++) {
                                        unsigned int pixel_x =
                                            j * tile_size + x;
                                        unsigned int pixel_y =
                                            i * tile_size + y;
                                        if (pixel_x < width &&
                                            pixel_y < height) {
                                                wallpaper_pixels[pixel_y *
                                                                     width +
                                                                 pixel_x] =
                                                    pixel_color;
                                        }
                                }
                        }
                }
        }
}