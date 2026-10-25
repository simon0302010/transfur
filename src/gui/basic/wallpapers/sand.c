#include "sand.h"
#include "../wallpaper.h"
#include "../wrappers/basic_wrapper.h"

#define SAND_Y_GOAL                                                            \
        100 /* Keep picking points until sand is stacked up this high */

#define SAND_MIN_PARTICLES_PER_POINT 200
#define SAND_MAX_PARTICLES_PER_POINT 10000

#define SAND_PARTICLE_FALL_CHANCE                                              \
        0.6f /* Cut in half for left/right, then *(n) for n possible pixels    \
                to drop */

#define SAND_PARTICLE_MOVE_CHANCE                                              \
        0.3f /* Chance to move X by +- 1 (while falling) */

#define SAND_PARTICLE_COLOR_BRIGHTNESS_VARIATION 20

#define SAND_PARTICLE_SPAWN_ABOVE_BY                                           \
        14 /* Spawn particles this far above the current highest surface */

#define SAND_MIN_SCALE 2
#define SAND_MAX_SCALE 4

struct color sand_colors[] = {
    {255, 243, 214}, {226, 182, 126}, {198, 137, 88},  {138, 72, 36},
    {255, 45, 128},  {220, 20, 95},   {255, 95, 31},   {255, 200, 0},
    {34, 215, 122},  {16, 168, 92},   {162, 230, 65},  {0, 210, 255},
    {30, 144, 255},  {88, 86, 214},   {245, 166, 215}, {245, 250, 255}};

static struct color lerp_color(struct color c1, struct color c2, float t) {
        struct color result;
        result.r = (unsigned char)(c1.r + (c2.r - c1.r) * t);
        result.g = (unsigned char)(c1.g + (c2.g - c1.g) * t);
        result.b = (unsigned char)(c1.b + (c2.b - c1.b) * t);
        return result;
}

static struct color get_random_sand_color(void) {
        return sand_colors[(
            int)(basic_wallpaper_get_random() *
                 (int)(sizeof(sand_colors) / sizeof(sand_colors[0])))];
}

static tbool pixel_at_point(struct color *wallpaper_pixels, unsigned int width,
                            unsigned int height, unsigned int x,
                            unsigned int y) {
        struct color pixel_color;

        if (x >= width || y >= height) {
                return false;
        }

        pixel_color = wallpaper_pixels[y * width + x];
        return (pixel_color.r != 0 || pixel_color.g != 0 || pixel_color.b != 0);
}

static float fmax(float a, float b) { return (a > b) ? a : b; }

static float fmin(float a, float b) { return (a < b) ? a : b; }

void basic_draw_sand_wallpaper(struct color *wallpaper_pixels,
                               unsigned int width, unsigned int height) {
        struct color current_color = get_random_sand_color();
        struct color next_color = get_random_sand_color();

        unsigned int x, y, i;

        unsigned int scale =
            (unsigned int)(SAND_MIN_SCALE +
                           basic_wallpaper_get_random() *
                               (SAND_MAX_SCALE - SAND_MIN_SCALE));

        tbool goal_met = false;

        /* Reset wallpaper_pixels to black */
        for (y = 0; y < height; y++) {
                for (x = 0; x < width; x++) {
                        wallpaper_pixels[y * width + x].r = 0;
                        wallpaper_pixels[y * width + x].b = 0;
                        wallpaper_pixels[y * width + x].g = 0;
                }
        }

        while (!goal_met) {
                unsigned int point_x =
                    (unsigned int)(basic_wallpaper_get_random() * width) *
                    scale;

                unsigned int point_pixels =
                    (unsigned int)(SAND_MIN_PARTICLES_PER_POINT +
                                   basic_wallpaper_get_random() *
                                       (SAND_MAX_PARTICLES_PER_POINT -
                                        SAND_MIN_PARTICLES_PER_POINT));

                unsigned int min_y = height;

                for (i = 0; i < point_pixels; i++) {
                        unsigned int left_max_drop = 0;
                        unsigned int right_max_drop = 0;
                        float left_drop_chance =
                            SAND_PARTICLE_FALL_CHANCE / 2.0f;
                        float right_drop_chance =
                            SAND_PARTICLE_FALL_CHANCE / 2.0f;
                        struct color pixel_color =
                            lerp_color(current_color, next_color,
                                       (float)i / (float)point_pixels);

                        int color_brightness_variation =
                            (int)(basic_wallpaper_get_random() *
                                  SAND_PARTICLE_COLOR_BRIGHTNESS_VARIATION *
                                  2) -
                            SAND_PARTICLE_COLOR_BRIGHTNESS_VARIATION;

                        unsigned int dx, dy;

                        pixel_color.r = (unsigned char)fmax(
                            0, fmin(255, pixel_color.r +
                                             color_brightness_variation));
                        pixel_color.g = (unsigned char)fmax(
                            0, fmin(255, pixel_color.g +
                                             color_brightness_variation));
                        pixel_color.b = (unsigned char)fmax(
                            0, fmin(255, pixel_color.b +
                                             color_brightness_variation));

                        x = point_x;
                        if (min_y < SAND_PARTICLE_SPAWN_ABOVE_BY) {
                                y = 0;
                        } else {
                                y = min_y -
                                    SAND_PARTICLE_SPAWN_ABOVE_BY * scale;
                        }

                        /* Main falling */
                        while (!pixel_at_point(wallpaper_pixels, width, height,
                                               x, y) &&
                               y < height - 1) {
                                y += scale;
                                if (basic_wallpaper_get_random() <
                                    SAND_PARTICLE_MOVE_CHANCE) {
                                        if (basic_wallpaper_get_random() <
                                            0.5f) {
                                                if (x > 0)
                                                        x -= scale;
                                        } else {
                                                if (x < width - scale)
                                                        x += scale;
                                        }
                                }
                        }
                        y -= scale;

                        /* Slope falling */
                        /* left */
                        if (x > 0) {
                                while (!pixel_at_point(wallpaper_pixels, width,
                                                       height, x - scale,
                                                       y + left_max_drop *
                                                               scale) &&
                                       (y + left_max_drop * scale) <
                                           height - scale) {
                                        left_max_drop++;
                                }
                        }
                        /* right */
                        if (x < width - scale) {
                                while (!pixel_at_point(wallpaper_pixels, width,
                                                       height, x + scale,
                                                       y + right_max_drop *
                                                               scale) &&
                                       (y + right_max_drop * scale) <
                                           height - scale) {
                                        right_max_drop++;
                                }
                        }

                        left_drop_chance += left_max_drop * 0.1f;
                        right_drop_chance += right_max_drop * 0.1f;

                        if (left_drop_chance > 1.0f) {
                                left_drop_chance = 1.0f;
                        } else if (right_drop_chance > 1.0f) {
                                right_drop_chance = 1.0f;
                        }

                        if (basic_wallpaper_get_random() > 0.5f) {
                                if (basic_wallpaper_get_random() <
                                    left_drop_chance) {
                                        x -= scale;
                                        y += left_max_drop * scale;
                                } else if (basic_wallpaper_get_random() <
                                           right_drop_chance) {
                                        x += scale;
                                        y += right_max_drop * scale;
                                }
                        } else {
                                if (basic_wallpaper_get_random() <
                                    right_drop_chance) {
                                        x += scale;
                                        y += right_max_drop * scale;
                                } else if (basic_wallpaper_get_random() <
                                           left_drop_chance) {
                                        x -= scale;
                                        y += left_max_drop * scale;
                                }
                        }

                        for (dy = 0; dy < scale; dy++) {
                                for (dx = 0; dx < scale; dx++) {
                                        if (x + dx < width && y + dy < height) {
                                                wallpaper_pixels[(y + dy) *
                                                                     width +
                                                                 (x + dx)] =
                                                    pixel_color;
                                        }
                                }
                        }

                        if (y < min_y) {
                                min_y = y;
                        }

                        if (y <= (unsigned int)SAND_Y_GOAL) {
                                goal_met = true;
                        }
                }

                current_color = next_color;
                next_color = get_random_sand_color();
        }
}