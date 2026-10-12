#include "images.h"
#include "../../misc/bool.h"
#include <stdint.h>

#ifdef ENABLE_THEMES

#define X_EXTERN(id, name)                                                     \
        extern const unsigned char name##_bmp_data[];                          \
        extern const unsigned int name##_bmp_size;
IMAGE_LIST(X_EXTERN)
#undef X_EXTERN

struct image_entry {
        const unsigned char *raw_data;
        const unsigned int *raw_size;
        tbool loaded;
        uint32_t offset;
        int width;
        int height;
        uint16_t bpp;
};

#define X_ENTRY(id, name)                                                      \
        {name##_bmp_data, &name##_bmp_size, false, 0, 0, 0, 0},
static struct image_entry image_registry[IMAGE_COUNT] = {IMAGE_LIST(X_ENTRY)};
#undef X_ENTRY

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

void draw_image_scaled_cropped(basic_image_id_t id, int x, int y, int w, int h,
                               int min_x, int min_y, int max_x, int max_y) {
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

                if ((y + dy) < min_y || (y + dy) > max_y)
                        continue;

                for (dx = 0; dx < w; dx++) {
                        int src_x = (dx * img->width) / w;
                        int pixel_index = img->offset + bmp_y * row_stride +
                                          src_x * (img->bpp / 8);
                        struct color pixel_color;

                        if ((x + dx) < min_x || (x + dx) > max_x)
                                continue;

                        if (img->bpp == 24 || img->bpp == 32) {
                                /* bgr / bgra */
                                /* skip transparent pixels */
                                if (img->bpp == 32 &&
                                    img->raw_data[pixel_index + 3] == 0)
                                        continue;

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

void draw_image_scaled(basic_image_id_t id, int x, int y, int w, int h) {
        draw_image_scaled_cropped(id, x, y, w, h, x, y, x + w, y + h);
}

void draw_image_cropped(basic_image_id_t id, int x, int y, int min_x, int min_y,
                        int max_x, int max_y) {
        int w = get_image_width(id);
        int h = get_image_height(id);
        draw_image_scaled_cropped(id, x, y, w, h, min_x, min_y, max_x, max_y);
}

void draw_image(basic_image_id_t id, int x, int y) {
        int w = get_image_width(id);
        int h = get_image_height(id);
        draw_image_scaled_cropped(id, x, y, w, h, x, y, x + w, x + h);
}

void draw_image_tiled(basic_image_id_t id, int x, int y, int w, int h,
                      int tile_w, int tile_h) {
        int x_offset = -(w % tile_w) / 2;
        int y_offset = -(h % tile_h) / 2;
        int num_horiz = (w - x_offset + tile_w - 1) / tile_w;
        int num_vert = (h - y_offset + tile_h - 1) / tile_h;
        int i, j;

        for (i = 0; i < num_horiz; i++) {
                for (j = 0; j < num_vert; j++) {
                        draw_image_scaled_cropped(id, x + x_offset + i * tile_w,
                                                  y + y_offset + j * tile_h,
                                                  tile_w, tile_h, x, y, x + w,
                                                  y + h);
                }
        }
}

#endif