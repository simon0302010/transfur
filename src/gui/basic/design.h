#ifndef DESIGN_H
#define DESIGN_H

#include "wrappers/basic_wrapper.h"

#define BORDER_WIDTH 2
#define CONTAINER_MARGIN 8
#define BUTTON_MARGIN 8
#define TEXT_SIZE_TITLE 4
#define TEXT_SIZE_HEADING 3
#define TEXT_SIZE_REGULAR 2

/* This should be used across all elements */
#define BORDER_ROUNDED 0.4
#define PADDING 2

extern const struct color page_background;
extern const struct color text_color;
extern const struct color text_color_inverted;
extern const struct color primary_button_color;
extern const struct color failure_button_color;
extern const struct color success_button_color;
extern const struct color disabled_button_color;
extern const struct color border_color;
extern const struct color focus_color;

#endif /* DESIGN_H */
