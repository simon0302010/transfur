#ifndef BASIC_IMAGES_H
#define BASIC_IMAGES_H

#include "wrappers/basic_wrapper.h"

#ifdef ENABLE_THEMES

/* X(ENUM_SUFFIX, asset_symbol_prefix)*/
#define IMAGE_LIST(X)                                                          \
        X(COFFEE_BACKGROUND, coffee_background)                                \
        X(COFFEE_BUTTON_PRIMARY, coffee_button_primary)                        \
        X(COFFEE_BUTTON_DISABLED, coffee_button_disabled)                      \
        X(COFFEE_BUTTON_SUCCESS, coffee_button_success)                        \
        X(COFFEE_BUTTON_FAILURE, coffee_button_failure)                        \
        X(COFFEE_BORDERS_HORIZONTAL, coffee_borders_horizontal)                \
        X(COFFEE_BORDERS_VERTICAL, coffee_borders_vertical)                    \
        X(COFFEE_INPUT, coffee_input)                                          \
        X(COFFEE_PROGRESS_BAR_BACKGROUND, coffee_progress_bar_background)      \
        X(COFFEE_PROGRESS_BAR_SEGMENT, coffee_progress_bar_segment)            \
        X(COFFEE_PROGRESS_BAR_END, coffee_progress_bar_end)                    \
        X(PASTEL_BACKGROUND, pastel_background)                                \
        X(PASTEL_BUTTON_PRIMARY, pastel_button_primary)                        \
        X(PASTEL_BUTTON_DISABLED, pastel_button_disabled)                      \
        X(PASTEL_BUTTON_SUCCESS, pastel_button_success)                        \
        X(PASTEL_BUTTON_FAILURE, pastel_button_failure)                        \
        X(PASTEL_BORDERS_HORIZONTAL, pastel_borders_horizontal)                \
        X(PASTEL_BORDERS_VERTICAL, pastel_borders_vertical)                    \
        X(PASTEL_INPUT, pastel_input)                                          \
        X(PASTEL_PROGRESS_BAR_BACKGROUND, pastel_progress_bar_background)      \
        X(PASTEL_PROGRESS_BAR_SEGMENT, pastel_progress_bar_segment)            \
        X(PASTEL_PROGRESS_BAR_END, pastel_progress_bar_end)                    \
        X(GERMAN_BACKGROUND, german_background)                                \
        X(GERMAN_BUTTON_PRIMARY, german_button_primary)                        \
        X(GERMAN_BUTTON_DISABLED, german_button_disabled)                      \
        X(GERMAN_BUTTON_SUCCESS, german_button_success)                        \
        X(GERMAN_BUTTON_FAILURE, german_button_failure)                        \
        X(GERMAN_BORDERS_HORIZONTAL, german_borders_horizontal)                \
        X(GERMAN_BORDERS_VERTICAL, german_borders_vertical)                    \
        X(GERMAN_INPUT, german_input)                                          \
        X(GERMAN_PROGRESS_BAR_BACKGROUND, german_progress_bar_background)      \
        X(GERMAN_PROGRESS_BAR_SEGMENT, german_progress_bar_segment)            \
        X(GERMAN_PROGRESS_BAR_END, german_progress_bar_end)                    \
        X(CLASSIC_ICON, classic_icon)                                          \
        X(CLASSIC_LIGHT_ICON, classic_light_icon)                              \
        X(COFFEE_ICON, coffee_icon)                                            \
        X(PASTEL_ICON, pastel_icon)                                            \
        X(GERMAN_ICON, german_icon)                                            \
        X(AMERICAN_BACKGROUND, american_background)                            \
        X(AMERICAN_BUTTON_PRIMARY, american_button_primary)                    \
        X(AMERICAN_BUTTON_DISABLED, american_button_disabled)                  \
        X(AMERICAN_BUTTON_SUCCESS, american_button_success)                    \
        X(AMERICAN_BUTTON_FAILURE, american_button_failure)                    \
        X(AMERICAN_BORDERS_HORIZONTAL, american_borders_horizontal)            \
        X(AMERICAN_BORDERS_VERTICAL, american_borders_vertical)                \
        X(AMERICAN_INPUT, american_input)                                      \
        X(AMERICAN_PROGRESS_BAR_BACKGROUND, american_progress_bar_background)  \
        X(AMERICAN_PROGRESS_BAR_SEGMENT, american_progress_bar_segment)        \
        X(AMERICAN_PROGRESS_BAR_END, american_progress_bar_end)                \
        X(AMERICAN_ICON, american_icon)                                        \
        X(CASINO_BACKGROUND, casino_background)                                \
        X(CASINO_BUTTON_PRIMARY, casino_button_primary)                        \
        X(CASINO_BUTTON_DISABLED, casino_button_disabled)                      \
        X(CASINO_BUTTON_SUCCESS, casino_button_success)                        \
        X(CASINO_BUTTON_FAILURE, casino_button_failure)                        \
        X(CASINO_BORDERS_HORIZONTAL, casino_borders_horizontal)                \
        X(CASINO_BORDERS_VERTICAL, casino_borders_vertical)                    \
        X(CASINO_INPUT, casino_input)                                          \
        X(CASINO_PROGRESS_BAR_BACKGROUND, casino_progress_bar_background)      \
        X(CASINO_PROGRESS_BAR_SEGMENT, casino_progress_bar_segment)            \
        X(CASINO_PROGRESS_BAR_END, casino_progress_bar_end)                    \
        X(CASINO_ICON, casino_icon)

#define X_ENUM(id, name) IMAGE_##id,
typedef enum { IMAGE_LIST(X_ENUM) IMAGE_COUNT } basic_image_id_t;
#undef X_ENUM

void draw_image(basic_image_id_t id, int x, int y);
void draw_image_scaled(basic_image_id_t id, int x, int y, int w, int h);
void draw_image_scaled_cropped(basic_image_id_t id, int x, int y, int w, int h,
                               int min_x, int min_y, int max_x, int max_y);
void draw_image_cropped(basic_image_id_t id, int x, int y, int min_x, int min_y,
                        int max_x, int max_y);
void draw_image_tiled(basic_image_id_t id, int x, int y, int w, int h,
                      int tile_w, int tile_h);
int get_image_width(basic_image_id_t id);
int get_image_height(basic_image_id_t id);

#else

typedef enum { IMAGE_COUNT } basic_image_id_t;

#endif

#endif /* BASIC_IMAGES_H */