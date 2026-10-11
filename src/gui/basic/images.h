#ifndef BASIC_IMAGES_H
#define BASIC_IMAGES_H

#include "sdl3/basic_wrapper.h"

#ifdef ENABLE_THEMES

typedef enum { IMAGE_TEST_FONT, IMAGE_END } basic_image_id_t;

void draw_image(basic_image_id_t id, int x, int y);
void draw_image_scaled(basic_image_id_t id, int x, int y, int w, int h);
int get_image_width(basic_image_id_t id);
int get_image_height(basic_image_id_t id);

#endif

#endif /* BASIC_IMAGES_H */