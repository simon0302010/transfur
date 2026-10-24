#ifndef BASIC_WALLPAPER_COLORS_H
#define BASIC_WALLPAPER_COLORS_H

#include "../wrappers/basic_wrapper.h"

void basic_draw_colors_wallpaper(struct color *wallpaper_pixels,
                                 unsigned int width, unsigned int height);

void basic_draw_thematic_colors_wallpaper(struct color *wallpaper_pixels,
                                          unsigned int width,
                                          unsigned int height);

#endif /* BASIC_WALLPAPER_COLORS_H */