#include "theme.h"

#ifdef ENABLE_THEMES

static const struct basic_theme_palette palette_classic = {
    {0, 0, 0},     {255, 255, 255}, {0, 0, 0},
    {0, 100, 200}, {50, 200, 50},   {200, 50, 50},
    {50, 50, 50},  {200, 200, 200}, {200, 255, 0}};

static const struct basic_theme_palette palette_classic_light = {
    {240, 240, 240}, {20, 20, 20},    {255, 255, 255},
    {0, 120, 220},   {40, 180, 40},   {220, 40, 40},
    {180, 180, 180}, {100, 100, 100}, {0, 120, 255}};

static const struct basic_theme_palette palette_classic_inverted = {
    {0, 0, 0},     {20, 20, 20}, {0, 0, 0},       {0, 100, 200}, {50, 200, 50},
    {200, 50, 50}, {50, 50, 50}, {200, 200, 200}, {200, 255, 0}};

static const struct basic_theme_desc desc_CLASSIC = {
    THEME_COLOR, THEME_BG_SOLID_COLOR, 0,
    0,           IMAGE_CLASSIC_ICON,   palette_classic};

static const struct basic_theme_desc desc_CLASSIC_LIGHT = {
    THEME_COLOR, THEME_BG_SOLID_COLOR,     0,
    0,           IMAGE_CLASSIC_LIGHT_ICON, palette_classic_light};

static const struct basic_theme_desc desc_COFFEE = {
    THEME_IMAGE, THEME_BG_IMAGE_TILED, 32,
    32,          IMAGE_COFFEE_ICON,    palette_classic};

static const struct basic_theme_desc desc_PASTEL = {
    THEME_IMAGE, THEME_BG_IMAGE_TILED, 32,
    32,          IMAGE_PASTEL_ICON,    palette_classic_inverted};

static const struct basic_theme_desc desc_GERMAN = {
    THEME_IMAGE, THEME_BG_IMAGE_SCALED, 32,
    32,          IMAGE_GERMAN_ICON,     palette_classic};

static const struct basic_theme_desc desc_AMERICAN = {
    THEME_IMAGE, THEME_BG_IMAGE_TILED, 256,
    135,         IMAGE_AMERICAN_ICON,  palette_classic_inverted};

static basic_theme_t theme = THEME_CLASSIC;

basic_theme_t basic_get_theme(void) { return theme; }

const struct basic_theme_desc *basic_get_theme_desc(basic_theme_t theme) {
        switch (theme) {
#define X_THEME_DESC(name, type, arg)                                          \
        case THEME_##name:                                                     \
                return &desc_##name;
                THEME_LIST(X_THEME_DESC, 0)
#undef X_THEME_DESC
        default:
                return &desc_CLASSIC;
        }
}

const struct basic_theme_palette *basic_get_theme_palette(basic_theme_t theme) {
        return &basic_get_theme_desc(theme)->palette;
}

basic_theme_type_t basic_get_theme_type(basic_theme_t theme) {
        return basic_get_theme_desc(theme)->type;
}

basic_image_id_t basic_get_theme_icon(basic_theme_t theme) {
        return basic_get_theme_desc(theme)->icon;
}

basic_theme_t basic_theme_offset(basic_theme_t base, int offset) {
        int count = (int)THEME_END;
        int index = ((int)base + offset) % count;
        if (index < 0)
                index += count;

        return (basic_theme_t)index;
}

void basic_set_theme(basic_theme_t new_theme) { theme = new_theme; }

void basic_toggle_theme(void) { theme = basic_theme_offset(theme, 1); }

#endif