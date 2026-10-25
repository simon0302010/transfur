#include "quilt.h"
#include "../images.h"
#include "../wallpaper.h"
#include "../wrappers/basic_wrapper.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#define NUM_QUILT_LINES 10
#define NUM_QUILT_LINE_JAGS 25
#define QUILT_RESOLUTION 80
#define QUILT_STITCHES_PER_RESOLUTION 1
#define QUILT_STITCH_RADIUS 4

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

        size_t i, j, k;

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
                                        float cx =
                                            line_x + start_offset + dx * t;
                                        float cy = start_y + dy * t;

                                        for (s = -QUILT_STITCH_RADIUS;
                                             s <= QUILT_STITCH_RADIUS; s++) {
                                                int stitch_x =
                                                    (int)(cx + nx * s);
                                                int stitch_y =
                                                    (int)(cy + ny * s);
                                                if (stitch_x >= 0 &&
                                                    stitch_x < (int)width &&
                                                    stitch_y >= 0 &&
                                                    stitch_y < (int)height) {
                                                        wallpaper_pixels
                                                            [stitch_y * width +
                                                             stitch_x]
                                                                .r = 220;
                                                        wallpaper_pixels
                                                            [stitch_y * width +
                                                             stitch_x]
                                                                .g = 220;
                                                        wallpaper_pixels
                                                            [stitch_y * width +
                                                             stitch_x]
                                                                .b = 200;
                                                }
                                        }
                                }
                        }

                        for (k = 0; k < pixels_per_resolution_y; k++) {
                                float progress =
                                    (float)k / (float)pixels_per_resolution_y;
                                int y = start_y + k;
                                int x = line_x + start_offset + dx * progress;
                                if (x >= 0 && x < (int)width && y >= 0 &&
                                    y < (int)height) {
                                        wallpaper_pixels[y * width + x].r = 20;
                                        wallpaper_pixels[y * width + x].g = 20;
                                        wallpaper_pixels[y * width + x].b = 20;
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
                                        float cx =
                                            start_x + start_offset + dx * t;
                                        float cy =
                                            line_y + start_offset + dy * t;

                                        for (s = -QUILT_STITCH_RADIUS;
                                             s <= QUILT_STITCH_RADIUS; s++) {
                                                int stitch_x =
                                                    (int)(cx + nx * s);
                                                int stitch_y =
                                                    (int)(cy + ny * s);
                                                if (stitch_x >= 0 &&
                                                    stitch_x < (int)width &&
                                                    stitch_y >= 0 &&
                                                    stitch_y < (int)height) {
                                                        wallpaper_pixels
                                                            [stitch_y * width +
                                                             stitch_x]
                                                                .r = 220;
                                                        wallpaper_pixels
                                                            [stitch_y * width +
                                                             stitch_x]
                                                                .g = 220;
                                                        wallpaper_pixels
                                                            [stitch_y * width +
                                                             stitch_x]
                                                                .b = 200;
                                                }
                                        }
                                }
                        }

                        for (k = 0; k < pixels_per_resolution_x; k++) {
                                float progress =
                                    (float)k / (float)pixels_per_resolution_x;
                                int x = start_x + k;
                                int y = line_y + start_offset + dy * progress;
                                if (x >= 0 && x < (int)width && y >= 0 &&
                                    y < (int)height) {
                                        wallpaper_pixels[y * width + x].r = 20;
                                        wallpaper_pixels[y * width + x].g = 20;
                                        wallpaper_pixels[y * width + x].b = 20;
                                }
                        }
                }
        }

        free(horizontal_lines);
}

void basic_draw_debug_quilt_wallpaper(struct color *wallpaper_pixels,
                                      unsigned int width, unsigned int height) {
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

        size_t i, j, k;

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

        /* Debug- draw red vertical lines and blue horizontal lines atop a black
         * background */

        for (i = 0; i < height; i++) {
                for (j = 0; j < width; j++) {
                        wallpaper_pixels[i * width + j].r = 0;
                        wallpaper_pixels[i * width + j].g = 0;
                        wallpaper_pixels[i * width + j].b = 0;
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
                                        float cx =
                                            line_x + start_offset + dx * t;
                                        float cy = start_y + dy * t;

                                        for (s = -QUILT_STITCH_RADIUS;
                                             s <= QUILT_STITCH_RADIUS; s++) {
                                                int stitch_x =
                                                    (int)(cx + nx * s);
                                                int stitch_y =
                                                    (int)(cy + ny * s);
                                                if (stitch_x >= 0 &&
                                                    stitch_x < (int)width &&
                                                    stitch_y >= 0 &&
                                                    stitch_y < (int)height) {
                                                        wallpaper_pixels
                                                            [stitch_y * width +
                                                             stitch_x]
                                                                .r = 0;
                                                        wallpaper_pixels
                                                            [stitch_y * width +
                                                             stitch_x]
                                                                .g = 255;
                                                        wallpaper_pixels
                                                            [stitch_y * width +
                                                             stitch_x]
                                                                .b = 0;
                                                }
                                        }
                                }
                        }

                        for (k = 0; k < pixels_per_resolution_y; k++) {
                                float progress =
                                    (float)k / (float)pixels_per_resolution_y;
                                int y = start_y + k;
                                int x = line_x + start_offset + dx * progress;
                                if (x >= 0 && x < (int)width && y >= 0 &&
                                    y < (int)height) {
                                        wallpaper_pixels[y * width + x].r = 255;
                                        wallpaper_pixels[y * width + x].g = 0;
                                        wallpaper_pixels[y * width + x].b = 0;
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
                                        float cx =
                                            start_x + start_offset + dx * t;
                                        float cy =
                                            line_y + start_offset + dy * t;

                                        for (s = -QUILT_STITCH_RADIUS;
                                             s <= QUILT_STITCH_RADIUS; s++) {
                                                int stitch_x =
                                                    (int)(cx + nx * s);
                                                int stitch_y =
                                                    (int)(cy + ny * s);
                                                if (stitch_x >= 0 &&
                                                    stitch_x < (int)width &&
                                                    stitch_y >= 0 &&
                                                    stitch_y < (int)height) {
                                                        wallpaper_pixels
                                                            [stitch_y * width +
                                                             stitch_x]
                                                                .r = 0;
                                                        wallpaper_pixels
                                                            [stitch_y * width +
                                                             stitch_x]
                                                                .g = 255;
                                                        wallpaper_pixels
                                                            [stitch_y * width +
                                                             stitch_x]
                                                                .b = 0;
                                                }
                                        }
                                }
                        }

                        for (k = 0; k < pixels_per_resolution_x; k++) {
                                float progress =
                                    (float)k / (float)pixels_per_resolution_x;
                                int x = start_x + k;
                                int y = line_y + start_offset + dy * progress;
                                if (x >= 0 && x < (int)width && y >= 0 &&
                                    y < (int)height) {
                                        wallpaper_pixels[y * width + x].r = 0;
                                        wallpaper_pixels[y * width + x].g = 0;
                                        wallpaper_pixels[y * width + x].b = 255;
                                }
                        }
                }
        }

        free(horizontal_lines);
}