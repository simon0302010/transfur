#include "../../misc/bool.h"
#include <stdint.h>
#include "sdl3/basic_wrapper.h"
#include <string.h>
#include "text.h"

extern const unsigned char font_bmp_data[];
extern const unsigned int font_bmp_size;

#define FONT_ROWS 5
#define FONT_COLUMNS 19

uint32_t image_offset;
uint16_t image_width, image_height;
int char_width, char_height;
uint16_t bitCount;
tbool font_loaded = false;

void load_font(void) {
        /* Header
        2b - "BM"
        4b - size of the file
        2b - reserved; 0
        2b - reserved; 0
        4b - offset of image data
        */
        int cursor = 0;

        if (font_bmp_data[0] != 'B' || font_bmp_data[1] != 'M') {
                /* Header mismatch */
                /* idk what to do tho */
                /* TODO: maybe throw an error or something ? */
        }
        cursor += 2;

        /* Don't care about file size */
        cursor += 4;

        /* Skip reserved addresses (2*2b) */
        cursor += 4;

        /* Must read offset like this to support big-endian devices */
        image_offset =
            ((uint32_t)(unsigned char)font_bmp_data[cursor]) |
            ((uint32_t)(unsigned char)font_bmp_data[cursor + 1] << 8) |
            ((uint32_t)(unsigned char)font_bmp_data[cursor + 2] << 16) |
            ((uint32_t)(unsigned char)font_bmp_data[cursor + 3] << 24);
        cursor += 4;

        /* BITMAPINFOHEADER
        4b - size of this header
        2b - width of bitmap (unsigned 16-bit)
        2b - height of bitmap (unsigned 16-bit)
        2b - number of color planes
        2b - number of bits per pixel
        */
        cursor += 4;

        image_width = (uint32_t)font_bmp_data[cursor] |
                      ((uint32_t)font_bmp_data[cursor + 1] << 8) |
                      ((uint32_t)font_bmp_data[cursor + 2] << 16) |
                      ((uint32_t)font_bmp_data[cursor + 3] << 24);
        cursor += 4;

        image_height = (uint32_t)font_bmp_data[cursor] |
                       ((uint32_t)font_bmp_data[cursor + 1] << 8) |
                       ((uint32_t)font_bmp_data[cursor + 2] << 16) |
                       ((uint32_t)font_bmp_data[cursor + 3] << 24);
        cursor += 4;

        cursor += 2; /* skip biPlanes */

        bitCount = (uint16_t)font_bmp_data[cursor] |
                   ((uint16_t)font_bmp_data[cursor + 1] << 8);
        /* cursor += 2 */
        /* We don't really care about the other header data */

        char_width = image_width / FONT_COLUMNS;
        char_height = image_height / FONT_ROWS;

        font_loaded = true;
}

struct text_metrics draw_text(char *text, int x, int y, int size, struct color color) {
        int i = 0;
        int row_stride = ((image_width * bitCount / 8 + 3) & ~3);
        struct text_metrics metrics;

        if (!font_loaded) load_font();

        metrics.width = char_width * size * strlen(text); /* perhaps we should just count manually so we don't iterate this twice */
        metrics.height = char_height * size;

        while (text[i] != '\0') {
                int char_idx = (unsigned char)text[i] - ' ';
                int j, k;
                char map_x, map_y;

                if (char_idx < 0 || char_idx >= (FONT_COLUMNS * FONT_ROWS))
                        char_idx = '?' - ' ';

                map_x = char_idx % FONT_COLUMNS;
                map_y = char_idx / FONT_COLUMNS;

                for (k = 0; k < char_height; k++) {
                        int bmp_y = (image_height - 1) - (map_y * char_height + k);

                        for (j = 0; j < char_width; j++) {
                            int bmp_x = map_x * char_width + j;
                            int pixel_val;

                            /* the included font.bmp is 8bit */
                            if (bitCount == 8) {
                                pixel_val = font_bmp_data[image_offset + bmp_y * row_stride + bmp_x];
                            } /* TODO: consider adding support for other bit sizes */

                            if (pixel_val != 255) {
                                double intensity = (255.0 - pixel_val) / 255.0;
                                struct color pixel_color;
                                
                                int screen_x = x + (i * char_width + j) * size;
                                int screen_y = y + k * size;

                                pixel_color.r = (char)((double)color.r * intensity);
                                pixel_color.g = (char)((double)color.g * intensity);
                                pixel_color.b = (char)((double)color.b * intensity);

                                if (size > 1) {
                                        basic_fill_rect(screen_x, screen_y, size, size, pixel_color);
                                } else {
                                        basic_set_pixel(screen_x, screen_y, pixel_color);
                                }
                            }
                        }
                }

                i++;
        }

        return metrics;
}
