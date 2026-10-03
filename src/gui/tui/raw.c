#if !defined(OS_WINDOWS) && !defined(OS_MSDOS)

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

static struct termios original_termios;

#endif

void disable_raw_mode(void) {
#if !defined(OS_WINDOWS) && !defined(OS_MSDOS)
        printf("\x1b[?25h\x1b[?1049l");
        fflush(stdout);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
#endif
}

void enable_raw_mode(void) {
#if !defined(OS_WINDOWS) && !defined(OS_MSDOS)
        struct termios raw;
        tcgetattr(STDIN_FILENO, &original_termios);
        atexit(disable_raw_mode);

        raw = original_termios;

        raw.c_lflag &= ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

        printf("\x1b[?1049h\x1b[2J\x1b[H\x1b[?25l");
        fflush(stdout);
#endif
}