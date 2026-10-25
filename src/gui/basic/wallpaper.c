#include "wallpaper.h"

#ifdef ENABLE_THEMES

#include "stddef.h"
#include "stdlib.h"
#include "wallpapers/colors.h"
#include "wallpapers/quilt.h"
#include "wallpapers/sand.h"
#include "wallpapers/trig.h"
#include "wrappers/basic_wrapper.h"

static basic_wallpaper_type_t current_wallpaper_type = WALLPAPER_NONE;
static struct color *wallpaper_pixels = NULL;
static unsigned int wallpaper_width = 0;
static unsigned int wallpaper_height = 0;
static unsigned int wallpaper_seed = 0;
static unsigned int current_wallpaper_seed = 0;

void basic_wallpaper_init(int width, int height) {
        wallpaper_pixels = malloc(width * height * sizeof(struct color));
        wallpaper_width = width;
        wallpaper_height = height;
}

void basic_wallpaper_cleanup(void) {
        if (wallpaper_pixels) {
                free(wallpaper_pixels);
                wallpaper_pixels = NULL;
        }
        wallpaper_width = 0;
        wallpaper_height = 0;
}

unsigned int basic_wallpaper_get_seed(void) {
        current_wallpaper_seed =
            (current_wallpaper_seed * 1103515245 + 12345) & 0x7fffffff;
        return current_wallpaper_seed;
}

double basic_wallpaper_get_random(void) {
        return (double)basic_wallpaper_get_seed() / 0x7fffffff;
}

void basic_wallpaper_next(void) {
        current_wallpaper_type++;

        if (current_wallpaper_type >= WALLPAPER_COUNT) {
                current_wallpaper_type = WALLPAPER_NONE;
        }

        basic_wallpaper_regenerate();
}

void basic_wallpaper_regenerate(void) {
        wallpaper_seed = basic_wallpaper_get_seed();
        current_wallpaper_seed = wallpaper_seed;
        basic_wallpaper_draw();
}

void basic_wallpaper_draw(void) {
        switch (current_wallpaper_type) {
        case WALLPAPER_RANDOM_COLORS:
                basic_draw_colors_wallpaper(wallpaper_pixels, wallpaper_width,
                                            wallpaper_height);
                break;
        case WALLPAPER_RANDOM_THEMATIC_COLORS:
                basic_draw_thematic_colors_wallpaper(
                    wallpaper_pixels, wallpaper_width, wallpaper_height);
                break;
        case WALLPAPER_QUILT:
                basic_draw_quilt_wallpaper(wallpaper_pixels, wallpaper_width,
                                           wallpaper_height);
                break;
        case WALLPAPER_SAND:
                basic_draw_sand_wallpaper(wallpaper_pixels, wallpaper_width,
                                          wallpaper_height);
                break;
        case WALLPAPER_TRIG:
                basic_draw_trig_wallpaper(wallpaper_pixels, wallpaper_width,
                                          wallpaper_height);
                break;
        default:
                break;
        }
}

tbool basic_wallpaper_is_active(void) {
        return current_wallpaper_type != WALLPAPER_NONE;
}

void basic_wallpaper_render(void) {
        if (wallpaper_pixels && wallpaper_width > 0 && wallpaper_height > 0) {
                basic_draw_buffer(wallpaper_pixels, wallpaper_width,
                                  wallpaper_height);
        }
}

/* Redraw in case this wallpaper depends on theme colors, but leave seed since
 * user did not explicitly regenerate it*/
void basic_wallpaper_on_theme_changed(void) {
        current_wallpaper_seed = wallpaper_seed;
        basic_wallpaper_draw();
}

#endif