#if !defined(OS_WINDOWS) && !defined(OS_MSDOS)
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#elif defined(OS_MSDOS) && defined(__WATCOMC__)
#include <graph.h>
#elif defined(OS_MSDOS) || defined(OS_WINDOWS)
#include <conio.h>
#endif

/* Kept separate from the includes */
#if !defined(OS_WINDOWS) && !defined(OS_MSDOS)
static struct termios original_termios;
#endif

/* Just clears the screen on Windows and MS-DOS */
void disable_raw_mode(void) {
#if !defined(OS_WINDOWS) && !defined(OS_MSDOS)
        printf("\x1b[?25h\x1b[?1049l");
        fflush(stdout);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
#elif defined(OS_MSDOS) && defined(__WATCOMC__)
        _clearscreen(_GCLEARSCREEN);
#elif defined(OS_MSDOS) || defined(OS_WINDOWS)
        clrscr();
#endif
}

static void sigint_handler(int sig) {
        disable_raw_mode();
        _exit(128 + sig);
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

        signal(SIGINT, sigint_handler);
#elif defined(OS_MSDOS) && defined(__WATCOMC__)
        _clearscreen(_GCLEARSCREEN);
#elif defined(OS_MSDOS) || defined(OS_WINDOWS)
        clrscr();
#endif
}

/* use `fflush(stdout)` after running on posix systems */
void reset_cursor(void) {
#if !defined(OS_WINDOWS) && !defined(OS_MSDOS)
        printf("\x1b[H");
#elif defined(OS_MSDOS) && defined(__WATCOMC__)
        _settextposition(0, 0);
#elif defined(OS_MSDOS) || defined(OS_WINDOWS)
        gotoxy(0, 0);
#endif
}