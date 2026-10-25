#include "quilt.h"
#include "../images.h"
#include "../wallpaper.h"
#include "../wrappers/basic_wrapper.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#define NUM_QUILT_LINES 10
#define NUM_QUILT_LINE_JAGS 25
#define QUILT_RESOLUTION 70
#define QUILT_STITCHES_PER_RESOLUTION 1
#define QUILT_STITCH_RADIUS 5

#define QUILT_ACCEL_FORCE 1.2f
#define QUILT_DAMPING 0.85f
#define QUILT_SPRING 0.12f

#define TEXTURE_TILE_SRC_SIZE 128
#define QUILT_IMAGE_TILE_SIZE 140 /* Scale size */

#define QUILT_MAX_OFFSET                                                       \
        30 /* Just how much we guess a line will get from it's center */

static int
get_vert_line_x(int line_index, int y, int num_lines, int width,
                int horiz_spacing, int pixels_per_res_y,
                int vertical_lines[QUILT_RESOLUTION][NUM_QUILT_LINES]) {
        int j, k, start_offset, end_offset;
        float progress;

        if (line_index <= 0)
                return 0;
        if (line_index >= num_lines)
                return width;

        j = y / pixels_per_res_y;
        if (j >= QUILT_RESOLUTION - 1)
                j = QUILT_RESOLUTION - 2;
        k = y - j * pixels_per_res_y;
        progress = (float)k / (float)pixels_per_res_y;

        start_offset = vertical_lines[j][line_index];
        end_offset = vertical_lines[j + 1][line_index];

        return line_index * horiz_spacing +
               (int)(start_offset + (end_offset - start_offset) * progress);
}

static int get_horiz_line_y(int line_index, int x, int num_lines, int height,
                            int vert_spacing, int pixels_per_res_x,
                            const int *horizontal_lines) {
        int j, k, start_offset, end_offset;
        float progress;

        if (line_index <= 0)
                return 0;
        if (line_index >= num_lines)
                return height;

        j = x / pixels_per_res_x;
        if (j >= QUILT_RESOLUTION - 1)
                j = QUILT_RESOLUTION - 2;
        k = x - j * pixels_per_res_x;
        progress = (float)k / (float)pixels_per_res_x;

        start_offset = horizontal_lines[j + line_index * QUILT_RESOLUTION];
        end_offset = horizontal_lines[j + 1 + line_index * QUILT_RESOLUTION];

        return line_index * vert_spacing +
               (int)(start_offset + (end_offset - start_offset) * progress);
}

static struct color sample_tile_pixel(const struct image_entry *img,
                                      int tile_index, int local_x,
                                      int local_y) {
        struct color color;
        int source_u, source_v, bmp_y, source_x, pixel_index;

        source_u = (local_x * TEXTURE_TILE_SRC_SIZE) / QUILT_IMAGE_TILE_SIZE;
        source_v = (local_y * TEXTURE_TILE_SRC_SIZE) / QUILT_IMAGE_TILE_SIZE;

        if (source_u < 0)
                source_u = 0;
        if (source_v < 0)
                source_v = 0;
        if (source_u >= TEXTURE_TILE_SRC_SIZE)
                source_u = TEXTURE_TILE_SRC_SIZE - 1;
        if (source_v >= TEXTURE_TILE_SRC_SIZE)
                source_v = TEXTURE_TILE_SRC_SIZE - 1;

        bmp_y = (img->height - 1) - source_v;
        source_x = tile_index * TEXTURE_TILE_SRC_SIZE + source_u;

        pixel_index = img->offset +
                      bmp_y * ((img->width * (img->bpp / 8) + 3) & ~3) +
                      source_x * 3;

        color.b = img->raw_data[pixel_index];
        color.g = img->raw_data[pixel_index + 1];
        color.r = img->raw_data[pixel_index + 2];

        /* Since this is a background after all, slightly low brightness */
        color.b = (unsigned char)(color.b * 0.8f);
        color.g = (unsigned char)(color.g * 0.8f);
        color.r = (unsigned char)(color.r * 0.8f);

        return color;
}

/* stolen from ../drawing.c */
/* TODO: perhaps make a way for wallpaper functions and rendering functions to
 * interact? */

static void draw_line_low(struct color *wallpaper_pixels, unsigned int width,
                          unsigned int height, int x1, int y1, int x2, int y2,
                          struct color color) {
        int dx = x2 - x1;
        int dy = y2 - y1;
        int yi = 1;
        int d;
        int x;
        int y;

        if (dy < 0) {
                yi = -1;
                dy = -dy;
        }

        d = (2 * dy) - dx;
        y = y1;

        for (x = x1; x <= x2; x++) {
                if (x >= 0 && x < (int)width && y >= 0 && y < (int)height)
                        wallpaper_pixels[y * width + x] = color;
                if (d > 0) {
                        y = y + yi;
                        d = d + (2 * (dy - dx));
                } else {
                        d = d + 2 * dy;
                }
        }
}

static void draw_line_high(struct color *wallpaper_pixels, unsigned int width,
                           unsigned int height, int x1, int y1, int x2, int y2,
                           struct color color) {
        int dx = x2 - x1;
        int dy = y2 - y1;
        int xi = 1;
        int d;
        int x;
        int y;

        if (dx < 0) {
                xi = -1;
                dx = -dx;
        }

        d = (2 * dx) - dy;
        x = x1;

        for (y = y1; y <= y2; y++) {
                if (x >= 0 && x < (int)width && y >= 0 && y < (int)height)
                        wallpaper_pixels[y * width + x] = color;
                if (d > 0) {
                        x = x + xi;
                        d = d + (2 * (dx - dy));
                } else {
                        d = d + 2 * dx;
                }
        }
}

static void draw_line(struct color *wallpaper_pixels, unsigned int width,
                      unsigned int height, unsigned int thickness, int x1,
                      int y1, int x2, int y2, struct color color) {
        int offset;
        int start;
        int end;

        if (thickness == 0)
                return;

        start = -((int)thickness / 2);
        end = start + (int)thickness;

        for (offset = start; offset < end; offset++) {
                if (abs(y2 - y1) < abs(x2 - x1)) {
                        if (x1 > x2)
                                draw_line_low(wallpaper_pixels, width, height,
                                              x2, y2 + offset, x1, y1 + offset,
                                              color);
                        else
                                draw_line_low(wallpaper_pixels, width, height,
                                              x1, y1 + offset, x2, y2 + offset,
                                              color);
                } else {
                        if (y1 > y2)
                                draw_line_high(wallpaper_pixels, width, height,
                                               x2 + offset, y2, x1 + offset, y1,
                                               color);
                        else
                                draw_line_high(wallpaper_pixels, width, height,
                                               x1 + offset, y1, x2 + offset, y2,
                                               color);
                }
        }
}

static struct color stitch_color = {220, 220, 200};
static struct color outline_color = {20, 20, 20};

void basic_draw_quilt_wallpaper(struct color *wallpaper_pixels,
                                unsigned int width, unsigned int height) {
        int col, row, y, x;
        int total_tiles;
        struct image_entry img_data;

        unsigned int horizontal_spacing =
            width / NUM_QUILT_LINES; /* these lines are vertical, but go along
                                        the horizon */

        unsigned int vertical_spacing = horizontal_spacing;

        unsigned int num_horizontal_lines =
            (height + vertical_spacing - 1) / vertical_spacing;

        int vertical_lines[QUILT_RESOLUTION][NUM_QUILT_LINES];
        int *horizontal_lines = (int *)malloc(
            QUILT_RESOLUTION * num_horizontal_lines * sizeof(int));

        unsigned int pixels_per_resolution_y = height / QUILT_RESOLUTION;
        unsigned int pixels_per_resolution_x = width / QUILT_RESOLUTION;
        unsigned int resolutions_per_jag =
            QUILT_RESOLUTION / NUM_QUILT_LINE_JAGS;

        size_t i, j;

        if (!get_image_data(IMAGE_TEXTURES_STRIP, &img_data)) {
                free(horizontal_lines);
                return;
        }

        for (i = 0; i < NUM_QUILT_LINES; i++) {
                float offset = 0.0f;
                float velocity = 0.0f;
                float acceleration = 0.0f;
                for (j = 0; j < QUILT_RESOLUTION; j++) {
                        if (j % resolutions_per_jag == 0) {
                                acceleration =
                                    (float)(basic_wallpaper_get_random() * 2.0 -
                                            1.0) *
                                    QUILT_ACCEL_FORCE;
                        }

                        acceleration -= offset * QUILT_SPRING;

                        velocity += acceleration;
                        velocity *= QUILT_DAMPING;
                        offset += velocity;

                        vertical_lines[j][i] = (int)offset;
                }
        }

        for (i = 0; i < num_horizontal_lines; i++) {
                float offset = 0.0f;
                float velocity = 0.0f;
                float acceleration = 0.0f;
                for (j = 0; j < QUILT_RESOLUTION; j++) {
                        if (j % resolutions_per_jag == 0) {
                                acceleration =
                                    (float)(basic_wallpaper_get_random() * 2.0 -
                                            1.0) *
                                    QUILT_ACCEL_FORCE;
                        }

                        acceleration -= offset * QUILT_SPRING;

                        velocity += acceleration;
                        velocity *= QUILT_DAMPING;
                        offset += velocity;

                        horizontal_lines[j + i * QUILT_RESOLUTION] = offset;
                }
        }

        total_tiles = img_data.width / TEXTURE_TILE_SRC_SIZE;

        for (row = 0; row < num_horizontal_lines; row++) {
                for (col = 0; col < NUM_QUILT_LINES; col++) {
                        int tile_index =
                            (int)(basic_wallpaper_get_random() * total_tiles);
                        int origin_x = col * horizontal_spacing;
                        int origin_y = row * vertical_spacing;
                        int min_y = row * vertical_spacing - QUILT_MAX_OFFSET;
                        int max_y =
                            (row + 1) * vertical_spacing + QUILT_MAX_OFFSET;
                        if (min_y < 0)
                                min_y = 0;
                        if (max_y > (int)height)
                                max_y = (int)height;

                        for (y = min_y; y < max_y; y++) {
                                int x_left = get_vert_line_x(
                                    col, y, NUM_QUILT_LINES, width,
                                    horizontal_spacing, pixels_per_resolution_y,
                                    vertical_lines);
                                int x_right = get_vert_line_x(
                                    col + 1, y, NUM_QUILT_LINES, width,
                                    horizontal_spacing, pixels_per_resolution_y,
                                    vertical_lines);
                                if (x_left < 0)
                                        x_left = 0;
                                if (x_right > (int)width)
                                        x_right = (int)width;

                                for (x = x_left; x < x_right; x++) {
                                        int y_top = get_horiz_line_y(
                                            row, x, num_horizontal_lines,
                                            height, vertical_spacing,
                                            pixels_per_resolution_x,
                                            horizontal_lines);
                                        int y_bottom = get_horiz_line_y(
                                            row + 1, x, num_horizontal_lines,
                                            height, vertical_spacing,
                                            pixels_per_resolution_x,
                                            horizontal_lines);
                                        if (y_top < 0)
                                                y_top = 0;
                                        if (y_bottom > (int)height)
                                                y_bottom = (int)height;

                                        if (y >= y_top && y < y_bottom) {
                                                int local_x = x - origin_x;
                                                int local_y = y - origin_y;
                                                struct color pixel_color =
                                                    sample_tile_pixel(
                                                        &img_data, tile_index,
                                                        local_x, local_y);
                                                wallpaper_pixels[y * width +
                                                                 x] =
                                                    pixel_color;
                                        }
                                }
                        }
                }
        }

        for (i = 0; i < NUM_QUILT_LINES; i++) {
                unsigned int line_x = i * horizontal_spacing;
                for (j = 0; j < QUILT_RESOLUTION - 1; j++) {
                        int start_y = j * pixels_per_resolution_y;
                        int end_y = (j + 1) * pixels_per_resolution_y;
                        int start_offset = vertical_lines[j][i];
                        int end_offset = vertical_lines[j + 1][i];
                        int dy = end_y - start_y;
                        int dx = end_offset - start_offset;
                        float seg_len =
                            (float)sqrt((double)(dx * dx + dy * dy));

                        draw_line(wallpaper_pixels, width, height, 5,
                                  line_x + start_offset, start_y,
                                  line_x + end_offset, end_y, outline_color);

                        if (seg_len > 0.001f) {
                                float nx = -dy / seg_len;
                                float ny = dx / seg_len;
                                int stitch_i;
                                int s;

                                for (stitch_i = 0;
                                     stitch_i < QUILT_STITCHES_PER_RESOLUTION;
                                     stitch_i++) {
                                        float t =
                                            (stitch_i + 0.5f) /
                                            (float)
                                                QUILT_STITCHES_PER_RESOLUTION;
                                        float cx =
                                            line_x + start_offset + dx * t;
                                        float cy = start_y + dy * t;

                                        int x0 =
                                            (int)(cx +
                                                  nx * -QUILT_STITCH_RADIUS);
                                        int y0 =
                                            (int)(cy +
                                                  ny * -QUILT_STITCH_RADIUS);
                                        int x1 =
                                            (int)(cx +
                                                  nx * QUILT_STITCH_RADIUS);
                                        int y1 =
                                            (int)(cy +
                                                  ny * QUILT_STITCH_RADIUS);

                                        draw_line(wallpaper_pixels, width,
                                                  height, 2, x0, y0, x1, y1,
                                                  stitch_color);
                                }
                        }
                }
        }

        for (i = 0; i < num_horizontal_lines; i++) {
                unsigned int line_y = i * vertical_spacing;
                for (j = 0; j < QUILT_RESOLUTION - 1; j++) {
                        int start_x = j * pixels_per_resolution_x;
                        int end_x = (j + 1) * pixels_per_resolution_x;
                        int start_offset =
                            horizontal_lines[j + i * QUILT_RESOLUTION];
                        int end_offset =
                            horizontal_lines[j + 1 + i * QUILT_RESOLUTION];
                        int dx = end_x - start_x;
                        int dy = end_offset - start_offset;

                        float seg_len =
                            (float)sqrt((double)(dx * dx + dy * dy));

                        draw_line(wallpaper_pixels, width, height, 5, start_x,
                                  line_y + start_offset, end_x,
                                  line_y + end_offset, outline_color);

                        if (seg_len > 0.001f) {
                                float nx = -dy / seg_len;
                                float ny = dx / seg_len;
                                int stitch_i;
                                int s;

                                for (stitch_i = 0;
                                     stitch_i < QUILT_STITCHES_PER_RESOLUTION;
                                     stitch_i++) {
                                        float t =
                                            (float)stitch_i /
                                            (float)
                                                QUILT_STITCHES_PER_RESOLUTION;
                                        float cx = start_x + dx * t;
                                        float cy =
                                            line_y + start_offset + dy * t;

                                        int x0 =
                                            (int)(cx +
                                                  nx * -QUILT_STITCH_RADIUS);
                                        int y0 =
                                            (int)(cy +
                                                  ny * -QUILT_STITCH_RADIUS);
                                        int x1 =
                                            (int)(cx +
                                                  nx * QUILT_STITCH_RADIUS);
                                        int y1 =
                                            (int)(cy +
                                                  ny * QUILT_STITCH_RADIUS);

                                        draw_line(wallpaper_pixels, width,
                                                  height, 2, x0, y0, x1, y1,
                                                  stitch_color);
                                }
                        }
                }
        }

        free(horizontal_lines);
}