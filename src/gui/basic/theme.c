#include "theme.h"

#ifdef ENABLE_THEMES

static basic_theme_t theme = THEME_CLASSIC;

basic_theme_t basic_get_theme(void) { return theme; }

void basic_set_theme(basic_theme_t new_theme) { theme = new_theme; }

void basic_toggle_theme(void) {
        theme++;

        if (theme >= THEME_END)
                theme = THEME_CLASSIC;
}

#endif