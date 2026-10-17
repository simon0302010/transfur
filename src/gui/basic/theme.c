#include "theme.h"

#ifdef ENABLE_THEMES

static basic_theme_t theme = THEME_CLASSIC;

basic_theme_t basic_get_theme(void) { return theme; }

basic_theme_type_t basic_get_theme_type(basic_theme_t theme) {
        switch (theme) {
        case THEME_CLASSIC:
                return THEME_COLOR;
        case THEME_COFFEE:
        case THEME_PASTEL:
        case THEME_GERMAN:
                return THEME_IMAGE;
        default:
                return THEME_OTHER;
        }
}

void basic_set_theme(basic_theme_t new_theme) { theme = new_theme; }

void basic_toggle_theme(void) {
        theme++;

        if (theme >= THEME_END)
                theme = THEME_CLASSIC;
}

#endif