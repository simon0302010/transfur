#ifndef THEME_SWITCHER_H
#define THEME_SWITCHER_H

#ifdef ENABLE_THEMES

void theme_switcher_show(void);
void theme_switcher_update(double dt);
void theme_switcher_render(int width, int height);

#else
#define theme_switcher_show() ((void)0)
#define theme_switcher_update(dt) ((void)0)
#define theme_switcher_render(width, height) ((void)0)
#endif

#endif /* THEME_SWITCHER_H */