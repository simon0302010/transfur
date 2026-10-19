#ifndef DESIGN_H
#define DESIGN_H

#include "wrappers/basic_wrapper.h"

#define BORDER_WIDTH 2
#define CONTAINER_MARGIN 8
#define BUTTON_MARGIN 8
#define TEXT_SIZE_TITLE 4
#define TEXT_SIZE_HEADING 3
#define TEXT_SIZE_REGULAR 2
#define THEME_SWITCHER_ICON 48

/* This should be used across all elements */
#define BORDER_ROUNDED 0.4
#define PADDING 2

#ifdef ENABLE_THEMES

#include "theme.h"

#define page_background                                                        \
        (basic_get_theme_palette(basic_get_theme())->page_background)
#define text_color (basic_get_theme_palette(basic_get_theme())->text_color)
#define text_color_inverted                                                    \
        (basic_get_theme_palette(basic_get_theme())->text_color_inverted)
#define primary_button_color                                                   \
        (basic_get_theme_palette(basic_get_theme())->primary_button_color)
#define success_button_color                                                   \
        (basic_get_theme_palette(basic_get_theme())->success_button_color)
#define failure_button_color                                                   \
        (basic_get_theme_palette(basic_get_theme())->failure_button_color)
#define disabled_button_color                                                  \
        (basic_get_theme_palette(basic_get_theme())->disabled_button_color)
#define border_color (basic_get_theme_palette(basic_get_theme())->border_color)
#define focus_color (basic_get_theme_palette(basic_get_theme())->focus_color)
#else
extern const struct color page_background;
extern const struct color text_color;
extern const struct color text_color_inverted;
extern const struct color primary_button_color;
extern const struct color failure_button_color;
extern const struct color success_button_color;
extern const struct color disabled_button_color;
extern const struct color border_color;
extern const struct color focus_color;
#endif

#endif /* DESIGN_H */
