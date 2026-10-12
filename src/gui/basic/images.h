#ifndef BASIC_IMAGES_H
#define BASIC_IMAGES_H

#include "sdl3/basic_wrapper.h"

#ifdef ENABLE_THEMES

/* X(ENUM_SUFFIX, asset_symbol_prefix)*/
#define IMAGE_LIST(X)                                                          \
        X(COFFEE_BACKGROUND, coffee_background)                                \
        X(COFFEE_BUTTON_PRIMARY, coffee_button_primary)                        \
        X(COFFEE_BUTTON_DISABLED, coffee_button_disabled)                      \
        X(COFFEE_BUTTON_SUCCESS, coffee_button_success)                        \
        X(COFFEE_BUTTON_FAILURE, coffee_button_failure)                        \
        X(COFFEE_BORDERS_HORIZONTAL, coffee_borders_horizontal)                \
        X(COFFEE_BORDERS_VERTICAL, coffee_borders_vertical)

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

#endif

#endif /* BASIC_IMAGES_H */