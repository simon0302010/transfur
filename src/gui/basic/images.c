#include "images.h"
#include "../../misc/bool.h"
#include <stdint.h>

#ifdef ENABLE_THEMES

/* i think this will work, if my understanding of enums is right */
#define IMAGE_COUNT IMAGE_END

extern const unsigned char test_font_bmp_data[];
extern const unsigned int test_font_bmp_size;

struct image_entry {
        const unsigned char *raw_data;
        const unsigned int *raw_size;
        tbool loaded;
        uint32_t offset;
        int width;
        int height;
        uint16_t bpp;
};

static struct image_entry image_registry[IMAGE_COUNT] = {
    {test_font_bmp_data, &test_font_bmp_size, false, 0, 0, 0, 0}};

static void load_image(basic_image_id_t id) {
        struct image_entry *img;
        const unsigned char *data;
        int cursor;

        if (id < 0 || id >= IMAGE_COUNT)
                return;

        img = &image_registry[id];
        if (img->loaded)
                return;

        data = img->raw_data;
        if (data[0] != 'B' || data[1] != 'M')
                return;

        cursor = 10;

        img->offset = ((uint32_t)(unsigned char)data[cursor]) |
                      ((uint32_t)(unsigned char)data[cursor + 1] << 8) |
                      ((uint32_t)(unsigned char)data[cursor + 2] << 16) |
                      ((uint32_t)(unsigned char)data[cursor + 3] << 24);
        cursor += 8;

        img->width = (uint32_t)data[cursor] |
                     ((uint32_t)data[cursor + 1] << 8) |
                     ((uint32_t)data[cursor + 2] << 16) |
                     ((uint32_t)data[cursor + 3] << 24);
        cursor += 4;

        img->height = (uint32_t)data[cursor] |
                      ((uint32_t)data[cursor + 1] << 8) |
                      ((uint32_t)data[cursor + 2] << 16) |
                      ((uint32_t)data[cursor + 3] << 24);
        cursor += 4;

        cursor += 2; /* skip biPlanes */

        img->bpp = (uint16_t)data[cursor] | ((uint16_t)data[cursor + 1] << 8);

        img->loaded = true;
}

int get_image_width(basic_image_id_t id) {
        if (id < 0 || id >= IMAGE_COUNT)
                return 0;
        if (!image_registry[id].loaded)
                load_image(id);
        return image_registry[id].width;
}

int get_image_height(basic_image_id_t id) {
        if (id < 0 || id >= IMAGE_COUNT)
                return 0;
        if (!image_registry[id].loaded)
                load_image(id);
        return image_registry[id].height;
}

void draw_image_scaled(basic_image_id_t id, int x, int y, int w, int h) {
        struct image_entry *img;
        int row_stride;
        int dy, dx;

        if (id < 0 || id >= IMAGE_COUNT || w <= 0 || h <= 0)
                return;

        img = &image_registry[id];
        if (!img->loaded)
                load_image(id);

        if (!img->loaded || img->width <= 0 || img->height <= 0)
                return;

        row_stride = ((img->width * (img->bpp / 8) + 3) & ~3);

        for (dy = 0; dy < h; dy++) {
                int src_y = (dy * img->height) / h;
                /* bmp is stored bottom to top, so reverse it */
                int bmp_y = (img->height - 1) - src_y;

                for (dx = 0; dx < w; dx++) {
                        int src_x = (dx * img->width) / w;
                        int pixel_index = img->offset + bmp_y * row_stride +
                                          src_x * (img->bpp / 8);
                        struct color pixel_color;
                        if (img->bpp == 24 || img->bpp == 32) {
                                /* bgr / bgra */
                                pixel_color.b = img->raw_data[pixel_index];
                                pixel_color.g = img->raw_data[pixel_index + 1];
                                pixel_color.r = img->raw_data[pixel_index + 2];

                                basic_set_pixel(x + dx, y + dy, pixel_color);
                        } else if (img->bpp == 8) {
                                unsigned char val = img->raw_data[pixel_index];
                                pixel_color.r = val;
                                pixel_color.b = val;
                                pixel_color.g = val;

                                basic_set_pixel(x + dx, y + dy, pixel_color);
                        }
                }
        }
}

void draw_image(basic_image_id_t id, int x, int y) {
        int w = get_image_width(id);
        int h = get_image_height(id);
        draw_image_scaled(id, x, y, w, h);
}

#endif