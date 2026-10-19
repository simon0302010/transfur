#include "theme.h"
#include "design.h"

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

static const struct basic_theme_palette palette_catppuccin_frappe = {
    {65, 69, 89},    {198, 208, 245}, {35, 38, 52},
    {133, 193, 220}, {166, 209, 137}, {231, 130, 132},
    {41, 44, 60},    {48, 52, 70},    {229, 200, 144}};

static const struct basic_theme_palette palette_catppuccin_latte = {
    {204, 208, 218}, {76, 79, 105},   {220, 224, 232},
    {32, 159, 181},  {64, 160, 43},   {210, 15, 57},
    {230, 223, 239}, {239, 241, 245}, {223, 142, 29}};

static const struct basic_theme_palette palette_catppuccin_macchiato = {
    {54, 58, 79},    {202, 211, 245}, {24, 25, 38},
    {125, 196, 228}, {166, 218, 149}, {237, 135, 150},
    {30, 32, 48},    {36, 39, 58},    {238, 212, 159}};

static const struct basic_theme_palette palette_catppuccin_mocha = {
    {49, 50, 68},    {205, 214, 244}, {17, 17, 27},
    {116, 199, 236}, {166, 227, 161}, {243, 139, 168},
    {24, 24, 37},    {30, 30, 46},    {249, 226, 175}};

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

static const struct basic_theme_desc desc_CASINO = {
    THEME_IMAGE, THEME_BG_IMAGE_TILED, 32,
    32,          IMAGE_CASINO_ICON,    palette_classic};

static const struct basic_theme_desc desc_CATPPUCCIN_FRAPPE = {
    THEME_COLOR, THEME_BG_SOLID_COLOR, 32,
    32,          IMAGE_CLASSIC_ICON,   palette_catppuccin_frappe};

static const struct basic_theme_desc desc_CATPPUCCIN_MACCHIATO = {
    THEME_COLOR, THEME_BG_SOLID_COLOR, 32,
    32,          IMAGE_CLASSIC_ICON,   palette_catppuccin_macchiato};

static const struct basic_theme_desc desc_CATPPUCCIN_MOCHA = {
    THEME_COLOR, THEME_BG_SOLID_COLOR, 32,
    32,          IMAGE_CLASSIC_ICON,   palette_catppuccin_mocha};

static const struct basic_theme_desc desc_CATPPUCCIN_LATTE = {
    THEME_COLOR, THEME_BG_SOLID_COLOR, 32,
    32,          IMAGE_CLASSIC_ICON,   palette_catppuccin_latte};

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