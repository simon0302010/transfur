#ifndef BASIC_THEME_H
#define BASIC_THEME_H

#include "images.h"
#include "wrappers/basic_wrapper.h"

typedef enum {
        THEME_BG_SOLID_COLOR,
        THEME_BG_IMAGE_TILED,
        THEME_BG_IMAGE_SCALED
} basic_theme_bg_mode_t;

struct basic_theme_palette {
        struct color page_background;
        struct color text_color;
        struct color text_color_inverted;
        struct color primary_button_color;
        struct color success_button_color;
        struct color failure_button_color;
        struct color disabled_button_color;
        struct color border_color;
        struct color focus_color;
};

typedef enum { THEME_COLOR, THEME_IMAGE, THEME_OTHER } basic_theme_type_t;

struct basic_theme_desc {
        basic_theme_type_t type;
        basic_theme_bg_mode_t bg_mode;
        int bg_tile_w;
        int bg_tile_h;
        basic_image_id_t icon;
        struct basic_theme_palette palette;
};

#define THEME_LIST(X, arg)                                                     \
        X(CLASSIC, THEME_COLOR, arg)                                           \
        X(CLASSIC_LIGHT, THEME_COLOR, arg)                                     \
        X(COFFEE, THEME_IMAGE, arg)                                            \
        X(PASTEL, THEME_IMAGE, arg)                                            \
        X(GERMAN, THEME_IMAGE, arg)

#define X_THEME_ENUM(name, type, arg) THEME_##name,
typedef enum { THEME_LIST(X_THEME_ENUM, 0) THEME_END } basic_theme_t;
#undef X_THEME_ENUM

#define THEME_RESOLVE_THEME_IMAGE(name, suffix)                                \
        (basic_get_theme() == THEME_##name) ? IMAGE_##name##_##suffix:
#define THEME_RESOLVE_THEME_COLOR(name, suffix)
#define THEME_RESOLVE_THEME_OTHER(name, suffix)

#define X_THEME_RESOLVE(name, type, suffix) THEME_RESOLVE_##type(name, suffix)

#define BASIC_THEME_IMAGE(suffix)                                              \
        (THEME_LIST(X_THEME_RESOLVE, suffix) IMAGE_COFFEE_##suffix)

#ifdef ENABLE_THEMES

#include "images.h"

basic_theme_t basic_get_theme(void);
const struct basic_theme_desc *basic_get_theme_desc(basic_theme_t theme);
const struct basic_theme_palette *basic_get_theme_palette(basic_theme_t theme);
basic_theme_type_t basic_get_theme_type(basic_theme_t theme);
basic_image_id_t basic_get_theme_icon(basic_theme_t theme);
basic_theme_t basic_theme_offset(basic_theme_t base, int offset);
void basic_set_theme(basic_theme_t theme);
void basic_toggle_theme(void);
#else
#define basic_get_theme() (0)
#define basic_get_theme_desc(t) ((void *)0)
#define basic_get_theme_palette(t) (&palette_classic)
#define basic_get_theme_type(t) (0)
#define basic_set_theme(t) ((void)0)
#define basic_theme_offset(base, offset) (0)
#define basic_toggle_theme() ((void)0)
#define basic_get_theme_icon(t) (0)
#endif

#endif /* BASIC_THEME_H */