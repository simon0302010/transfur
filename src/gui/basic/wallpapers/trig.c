#include "../../../misc/math.h"
#include "../theme.h"
#include "../wallpaper.h"
#include "colors.h"
#include <math.h>
#include <stddef.h>

void basic_draw_trig_wallpaper(struct color *wallpaper_pixels,
                               unsigned int width, unsigned int height) {
        double brightness = basic_wallpaper_get_random();
        unsigned int r, g, b;
        unsigned int i, j;
        unsigned int tile_size =
            (unsigned int)(basic_wallpaper_get_random() * width * 0.04) + 1;
        unsigned int x, y;
        double offset = basic_wallpaper_get_random() * 2 * M_PI;
        double other_offset = basic_wallpaper_get_random() * 2 * M_PI;

        if (brightness < 0.2)
                brightness = 0.2;

        for (i = 0; i < height / tile_size + 1; i++) {
                int goal = (int)(basic_wallpaper_get_random() * 10) + 1;
                for (j = 0; j < width / tile_size + 1; j++) {
                        struct color pixel_color;
                        if (i % goal == 0 || j % goal == 0) {
                                r = (unsigned int)(((sin((double)j + offset)) +
                                                    1.0) /
                                                   2.0 * 255 * brightness);
                        } else {
                                r = (unsigned int)(((sin((double)i +
                                                         other_offset)) +
                                                    1.0) /
                                                   2.0 * 255 * brightness);
                        }

                        g = (unsigned int)(((cos((double)i + offset)) + 1.0) /
                                           2.0 * 255 * brightness);
                        b = (unsigned int)((
                            ((sin((double)j + other_offset)) + 1.0) / 2.0 *
                            255 * brightness));
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
