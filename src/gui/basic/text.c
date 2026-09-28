#include <string.h>

#include "sdl3/basic_wrapper.h"

#define CHAR_PATTERN_WIDTH 5
#define CHAR_PATTERN_MAX_HEIGHT 8
#define LETTER_SPACE 1

#define CHAR_PATTERN_UNKNOWN "0111010010000100010000100000000010"
#define CHAR_PATTERN_A "0111010001111111000110001"
#define CHAR_PATTERN_B "11110100011000111110100011000111110"
#define CHAR_PATTERN_C "011101000110000100001000101110"
#define CHAR_PATTERN_D "11110100011000110001100011000111110"
#define CHAR_PATTERN_E "1111110000111111000011111"
#define CHAR_PATTERN_F "1111110000111111000010000"
#define CHAR_PATTERN_G "011101000110000101101000101111"
#define CHAR_PATTERN_H "1000110001111111000110001"
#define CHAR_PATTERN_I "1111100100001000010011111"
#define CHAR_PATTERN_J "1111100001000011000101110"
#define CHAR_PATTERN_K "100011001010100010000010001010010001"
#define CHAR_PATTERN_L "1000010000100001000011111"
#define CHAR_PATTERN_M "1000111011101011000110001"
#define CHAR_PATTERN_N "1000111001101011001110001"
#define CHAR_PATTERN_O "0111010001100011000101110"
#define CHAR_PATTERN_P "11110100011000111110100001000010000"
#define CHAR_PATTERN_Q "011101000110001100010111100001"
#define CHAR_PATTERN_R "11110100011000111110110001010010010"
#define CHAR_PATTERN_S "011111000001100001100000111110"
#define CHAR_PATTERN_T "1111100100001000010000100"
#define CHAR_PATTERN_U "1000110001100010101001110"
#define CHAR_PATTERN_V "1000110001010100101000100"
#define CHAR_PATTERN_W "1000110101101011101111011"
#define CHAR_PATTERN_X "1000101010001000101010001"
#define CHAR_PATTERN_Y "100011000101010001000010000100"
#define CHAR_PATTERN_Z "1111100010001000100011111"
#define CHAR_PATTERN_0 "0111010011101011100101110"
#define CHAR_PATTERN_1 "0110010100001000010011111"
#define CHAR_PATTERN_2 "01110100010000100010001000100011111"
#define CHAR_PATTERN_3 "1111000001011110000111110"
#define CHAR_PATTERN_4 "000110010101001111110001000010"
#define CHAR_PATTERN_5 "1111110000111100000111110"
#define CHAR_PATTERN_6 "001110100010000111101000101110"
#define CHAR_PATTERN_7 "1111100010001000100010000"
#define CHAR_PATTERN_8 "0111010001011101000101110"
#define CHAR_PATTERN_9 "011101000111110100000100000111"
#define CHAR_PATTERN_EXCLAMATION "011100111000100001000000000100"

void draw_text(char *text, int x, int y, int size, struct color color) {
        int i = 0;

        while (true) {
                char pattern[CHAR_PATTERN_WIDTH * CHAR_PATTERN_MAX_HEIGHT];
                int j = 0;

                if (text[i] == '\0')
                        break;

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
                case 'd':
                case 'D':
                        strcpy(pattern, CHAR_PATTERN_D);
                        break;
                case 'e':
                case 'E':
                        strcpy(pattern, CHAR_PATTERN_E);
                        break;
                case 'f':
                case 'F':
                        strcpy(pattern, CHAR_PATTERN_F);
                        break;
                case 'g':
                case 'G':
                        strcpy(pattern, CHAR_PATTERN_G);
                        break;
                case 'h':
                case 'H':
                        strcpy(pattern, CHAR_PATTERN_H);
                        break;
                case 'i':
                case 'I':
                        strcpy(pattern, CHAR_PATTERN_I);
                        break;
                case 'j':
                case 'J':
                        strcpy(pattern, CHAR_PATTERN_J);
                        break;
                case 'k':
                case 'K':
                        strcpy(pattern, CHAR_PATTERN_K);
                        break;
                case 'l':
                case 'L':
                        strcpy(pattern, CHAR_PATTERN_L);
                        break;
                case 'm':
                case 'M':
                        strcpy(pattern, CHAR_PATTERN_M);
                        break;
                case 'n':
                case 'N':
                        strcpy(pattern, CHAR_PATTERN_N);
                        break;
                case 'o':
                case 'O':
                        strcpy(pattern, CHAR_PATTERN_O);
                        break;
                case 'p':
                case 'P':
                        strcpy(pattern, CHAR_PATTERN_P);
                        break;
                case 'q':
                case 'Q':
                        strcpy(pattern, CHAR_PATTERN_Q);
                        break;
                case 'r':
                case 'R':
                        strcpy(pattern, CHAR_PATTERN_R);
                        break;
                case 's':
                case 'S':
                        strcpy(pattern, CHAR_PATTERN_S);
                        break;
                case 't':
                case 'T':
                        strcpy(pattern, CHAR_PATTERN_T);
                        break;
                case 'u':
                case 'U':
                        strcpy(pattern, CHAR_PATTERN_U);
                        break;
                case 'v':
                case 'V':
                        strcpy(pattern, CHAR_PATTERN_V);
                        break;
                case 'w':
                case 'W':
                        strcpy(pattern, CHAR_PATTERN_W);
                        break;
                case 'x':
                case 'X':
                        strcpy(pattern, CHAR_PATTERN_X);
                        break;
                case 'y':
                case 'Y':
                        strcpy(pattern, CHAR_PATTERN_Y);
                        break;
                case 'z':
                case 'Z':
                        strcpy(pattern, CHAR_PATTERN_Z);
                        break;
                case ' ':
                        pattern[0] = '\0';
                        break;
                case '0':
                        strcpy(pattern, CHAR_PATTERN_0);
                        break;
                case '1':
                        strcpy(pattern, CHAR_PATTERN_1);
                        break;
                case '2':
                        strcpy(pattern, CHAR_PATTERN_2);
                        break;
                case '3':
                        strcpy(pattern, CHAR_PATTERN_3);
                        break;
                case '4':
                        strcpy(pattern, CHAR_PATTERN_4);
                        break;
                case '5':
                        strcpy(pattern, CHAR_PATTERN_5);
                        break;
                case '6':
                        strcpy(pattern, CHAR_PATTERN_6);
                        break;
                case '7':
                        strcpy(pattern, CHAR_PATTERN_7);
                        break;
                case '8':
                        strcpy(pattern, CHAR_PATTERN_8);
                        break;
                case '9':
                        strcpy(pattern, CHAR_PATTERN_9);
                        break;
                case '!':
                        strcpy(pattern, CHAR_PATTERN_EXCLAMATION);
                        break;
                default:
                        strcpy(pattern, CHAR_PATTERN_UNKNOWN);
                        break;
                }

                while (true) {
                        int col = j % CHAR_PATTERN_WIDTH;
                        int row = j / CHAR_PATTERN_WIDTH;
                        int base_x = x + size * (col + LETTER_SPACE * i +
                                                 CHAR_PATTERN_WIDTH * i);
                        int base_y = y + size * row;
                        int k;
                        int l;

                        if (pattern[j] == '\0')
                                break;

                        if (pattern[j] == '1') {
                                basic_fill_rect(base_x, base_y, size, size,
                                                color);
                        } else if (size > 1) {
                                bool has_up =
                                    (row > 0 &&
                                     pattern[(row - 1) * CHAR_PATTERN_WIDTH +
                                             col] == '1');
                                bool has_down =
                                    (pattern[(row + 1) * CHAR_PATTERN_WIDTH +
                                             col] == '1');
                                bool has_left =
                                    (col > 0 &&
                                     pattern[row * CHAR_PATTERN_WIDTH +
                                             (col - 1)] == '1');
                                bool has_right =
                                    (col < CHAR_PATTERN_WIDTH - 1 &&
                                     pattern[row * CHAR_PATTERN_WIDTH +
                                             (col + 1)] == '1');

                                if (has_down && has_left) {
                                        for (k = 0; k < size / 2; k++) {
                                                int row_y =
                                                    base_y + size - 1 - k;
                                                for (l = 0; l < (size / 2 - k);
                                                     l++) {
                                                        basic_set_pixel(
                                                            base_x + l, row_y,
                                                            color);
                                                }
                                        }
                                }

                                if (has_down && has_right) {
                                        for (k = 0; k < size / 2; k++) {
                                                int row_y =
                                                    base_y + size - 1 - k;
                                                for (l = 0; l < (size / 2 - k);
                                                     l++) {
                                                        basic_set_pixel(
                                                            base_x + size - 1 -
                                                                l,
                                                            row_y, color);
                                                }
                                        }
                                }

                                if (has_up && has_left) {
                                        for (k = 0; k < size / 2; k++) {
                                                int row_y = base_y + k;
                                                for (l = 0; l < (size / 2 - k);
                                                     l++) {
                                                        basic_set_pixel(
                                                            base_x + l, row_y,
                                                            color);
                                                }
                                        }
                                }

                                if (has_up && has_right) {
                                        for (k = 0; k < size / 2; k++) {
                                                int row_y = base_y + k;
                                                for (l = 0; l < (size / 2 - k);
                                                     l++) {
                                                        basic_set_pixel(
                                                            base_x + size - 1 -
                                                                l,
                                                            row_y, color);
                                                }
                                        }
                                }
                        }

                        j++;
                }

                i++;
        }
}