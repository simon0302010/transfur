#ifndef BASIC_THEME_H
#define BASIC_THEME_H

typedef enum {
        THEME_CLASSIC,
        THEME_COFFEE,
        THEME_PASTEL,
        THEME_GERMAN,
        THEME_END
} basic_theme_t;

typedef enum { THEME_COLOR, THEME_IMAGE, THEME_OTHER } basic_theme_type_t;

#ifdef ENABLE_THEMES

basic_theme_t basic_get_theme(void);
basic_theme_type_t basic_get_theme_type(basic_theme_t theme);
void basic_set_theme(basic_theme_t theme);
void basic_toggle_theme(void);
#else
#define basic_get_theme() (0)
#define basic_get_theme_type(t) (0)
#define basic_set_theme(t) ((void)0)
#define basic_toggle_theme() ((void)0)
#endif

#endif /* BASIC_THEME_H */