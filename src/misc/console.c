#if defined(OS_WINDOWS)
#include <windows.h>
#elif defined(OS_MSDOS)
#include <conio.h>
#else
#include <sys/ioctl.h>
#endif

#include "console.h"

struct consolesize get_console_size(void) {
        struct consolesize cs = {0, 0};

#if defined(OS_WINDOWS)
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        cs.rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        cs.columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
#elif defined(OS_MSDOS)
        struct text_info info;
        gettextinfo(&info);
        cs.rows = info.screenheight;
        cs.columns = info.screenwidth;
#else
        struct winsize max;
        if (ioctl(0, TIOCGWINSZ, &max) < 0)
                return cs;
        cs.rows = max.ws_row;
        cs.columns = max.ws_col;
#endif
        return cs;
}