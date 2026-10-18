#ifndef BASIC_THEME_H
#define BASIC_THEME_H

#define THEME_LIST(X, arg)                                                     \
        X(CLASSIC, THEME_COLOR, arg)                                           \
        X(COFFEE, THEME_IMAGE, arg)                                            \
        X(PASTEL, THEME_IMAGE, arg)                                            \
        X(GERMAN, THEME_IMAGE, arg)

#define X_THEME_ENUM(name, type, arg) THEME_##name,
typedef enum { THEME_LIST(X_THEME_ENUM, 0) THEME_END } basic_theme_t;
#undef X_THEME_ENUM

typedef enum { THEME_COLOR, THEME_IMAGE, THEME_OTHER } basic_theme_type_t;

#define THEME_RESOLVE_THEME_IMAGE(name, suffix)                                \
        (basic_get_theme() == THEME_##name) ? IMAGE_##name##_##suffix:
#define THEME_RESOLVE_THEME_COLOR(name, suffix)
#define THEME_RESOLVE_THEME_OTHER(name, suffix)

#define X_THEME_RESOLVE(name, type, suffix) THEME_RESOLVE_##type(name, suffix)

#define BASIC_THEME_IMAGE(suffix)                                              \
        (THEME_LIST(X_THEME_RESOLVE, suffix) IMAGE_COFFEE_##suffix)

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