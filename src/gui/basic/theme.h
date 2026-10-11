#ifndef BASIC_THEME_H
#define BASIC_THEME_H

typedef enum { THEME_CLASSIC, THEME_COFFEE, THEME_END } basic_theme_t;

#ifdef ENABLE_THEMES

basic_theme_t basic_get_theme(void);
void basic_set_theme(basic_theme_t theme);
void basic_toggle_theme(void);
#else
#define basic_get_theme() (0)
#define basic_set_theme(t) ((void)0)
#define basic_toggle_theme() ((void)0)
#endif

#endif /* BASIC_THEME_H */