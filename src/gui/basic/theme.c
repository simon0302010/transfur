#include "theme.h"

#ifdef ENABLE_THEMES

static basic_theme_t theme = THEME_CLASSIC;

basic_theme_t basic_get_theme(void) { return theme; }

basic_theme_type_t basic_get_theme_type(basic_theme_t theme) {
        switch (theme) {
#define X_THEME_CASE(name, type, arg)                                          \
        case THEME_##name:                                                     \
                return type;
                THEME_LIST(X_THEME_CASE, 0)
#undef X_THEME_CASE
        default:
                return THEME_OTHER;
        }
}

basic_image_id_t basic_get_theme_icon(basic_theme_t theme) {
        switch (theme) {
#define X_THEME_CASE(name, type, arg)                                          \
        case THEME_##name:                                                     \
                return IMAGE_##name##_ICON;
                THEME_LIST(X_THEME_CASE, 0)
#undef X_THEME_CASE
        default:
                return IMAGE_CLASSIC_ICON;
        }
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