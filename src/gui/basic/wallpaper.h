#ifndef BASIC_WALLPAPER_H
#define BASIC_WALLPAPER_H

#include "../../misc/bool.h"

typedef enum {
        WALLPAPER_NONE,
        WALLPAPER_RANDOM_COLORS,
        WALLPAPER_RANDOM_THEMATIC_COLORS,
        WALLPAPER_QUILT,
        WALLPAPER_SAND,
        WALLPAPER_COUNT
} basic_wallpaper_type_t;

#ifdef ENABLE_THEMES

void basic_wallpaper_init(int width, int height);
void basic_wallpaper_cleanup(void);
unsigned int basic_wallpaper_get_seed(void);
double basic_wallpaper_get_random(void);
void basic_wallpaper_next(void);
void basic_wallpaper_regenerate(void);
void basic_wallpaper_draw(void);
tbool basic_wallpaper_is_active(void);
void basic_wallpaper_render(void);
void basic_wallpaper_on_theme_changed(void);

#else

#define basic_wallpaper_init(w, h) ((void)0)
#define basic_wallpaper_cleanup() ((void)0)
#define basic_wallpaper_next() ((void)0)
#define basic_wallpaper_regenerate() ((void)0)
#define basic_wallpaper_draw() ((void)0)
#define basic_wallpaper_is_active() (false)
#define basic_wallpaper_render() ((void)0)
#define basic_wallpaper_on_theme_changed() ((void)0)

#endif

#endif /* BASIC_WALLPAPER_H */