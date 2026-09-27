#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

static struct termios original_termios;

void disable_raw_mode(void) {
        printf("\x1b[?25h\x1b[?1049l");
        fflush(stdout);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios); 
}

void enable_raw_mode(void) {
        struct termios raw;
        tcgetattr(STDIN_FILENO, &original_termios);
        atexit(disable_raw_mode);

        raw = original_termios;

        raw.c_lflag &= ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

        printf("\x1b[?1049h\x1b[2J\x1b[H\x1b[?25l");
        fflush(stdout);
}