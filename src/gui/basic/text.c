#include "../basic_wrapper/basic_wrapper.h"
#include <string.h>

#define CHAR_PATTERN_WIDTH 5
#define CHAR_PATTERN_MAX_HEIGHT 8
#define LETTER_SPACE 1

#define CHAR_PATTERN_UNKNOWN "0111010010000100010000100000000010"
#define CHAR_PATTERN_A "0111010001111111000110001"
#define CHAR_PATTERN_B "11110100011000111110100011000111110"
#define CHAR_PATTERN_C "011101000110000100001000101110"

void draw_text(char *text, int x, int y, int size, struct color color) {
        int i = 0;

        while (true) {
                char pattern[CHAR_PATTERN_WIDTH * CHAR_PATTERN_MAX_HEIGHT];
                int j = 0;

                if (text[i] == '\0') break;

                switch (text[i]) {
                        case 'a':
                        case 'A':
                                strcpy(pattern, CHAR_PATTERN_A);
                                break;
                        case 'b':
                        case 'B':
                                strcpy(pattern, CHAR_PATTERN_B);
                                break;
                        case 'c':
                        case 'C':
                                strcpy(pattern, CHAR_PATTERN_C);
                                break;
                        default:
                                strcpy(pattern, CHAR_PATTERN_UNKNOWN);
                                break;
                }

                while (true) {
                        if (pattern[j] == '\0') break;

                        if (pattern[j] == '1') {
                                int j_x = j % CHAR_PATTERN_WIDTH + LETTER_SPACE * i + CHAR_PATTERN_WIDTH * i;
                                int j_y = j / CHAR_PATTERN_WIDTH;
                                basic_fill_rect(x + size * j_x, y + size * j_y, size, size, color);
                        }

                        j++;
                }

                i++;
        }
}