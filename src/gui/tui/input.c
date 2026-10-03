#include "../../platform.h"

#if defined(OS_WINDOWS) || defined(OS_MSDOS)
#include "../../misc/ttime.h"
#include <conio.h>
#else
#include <sys/poll.h>
#include <unistd.h>
#endif

#include "tui.h"

/* Returns 1 for succes and anything below 1 for errors */
static int poll_key(char *c, int timeout_ms) {
#if defined(OS_WINDOWS)
        unsigned int elapsed = 0;

        while (elapsed < timeout_ms) {
                if (_kbhit()) {
                        *c = _getch();
                        return 1;
                }

                sleep_ms(10);
                elapsed += 10;
        }

        return -1;
#elif defined(OS_MSDOS)
        unsigned int elapsed = 0;

        while (elapsed < timeout_ms) {
                if (kbhit()) {
                        *c = getch();
                        return 1;
                }

                sleep_ms(10);
                elapsed += 10;
        }

        return -1;
#else
        int res;
        struct pollfd pfd;

        pfd.fd = STDIN_FILENO;
        pfd.events = POLLIN;

        res = poll(&pfd, 1, timeout_ms);
        if (!(res > 0 && (pfd.revents & POLLIN)))
                return -1;

        res = read(STDIN_FILENO, c, 1);
        return res;
#endif
}

struct tui_event poll_key_event(int timeout_ms) {
        struct tui_event event;
        char ch;
        int res;

        event.key = KEY_NONE;
        event.ch = '\0';

        if (poll_key(&ch, timeout_ms) <= 0)
                return event;

        if (ch == '\r' || ch == '\n') {
                event.key = KEY_ENTER;
        } else if (ch == '\t') {
                event.key = KEY_TAB;
        } else if (ch == 127 || ch == '\b') {
                event.key = KEY_BACKSPACE;
        } else if (ch == 27) {
#if !defined(OS_WINDOWS) && !defined(OS_MSDOS)
                struct pollfd pfd;
                char seq[3];

                pfd.fd = STDIN_FILENO;
                pfd.events = POLLIN;

                if (poll(&pfd, 1, 25) > 0 && (pfd.revents & POLLIN)) {
                        if (read(STDIN_FILENO, &seq[0], 1) > 0 &&
                            seq[0] == '[') {
                                if (read(STDIN_FILENO, &seq[1], 1) > 0) {
                                        switch (seq[1]) {
                                        case 'A':
                                                event.key = KEY_ARROW_UP;
                                                break;
                                        case 'B':
                                                event.key = KEY_ARROW_DOWN;
                                                break;
                                        case 'C':
                                                event.key = KEY_ARROW_RIGHT;
                                                break;
                                        case 'D':
                                                event.key = KEY_ARROW_LEFT;
                                                break;
                                        case 'Z':
                                                event.key = KEY_SHIFT_TAB;
                                                break;
                                        case '3':
                                                if (read(STDIN_FILENO, &seq[2],
                                                         1) > 0 &&
                                                    seq[2] == '~') {
                                                        event.key = KEY_DELETE;
                                                }
                                                break;
                                        default:
                                                break;
                                        }
                                }
                        }
                } else {
                        event.key = KEY_ESC;
                }
#else
                event.key = KEY_ESC;
#endif
        } else if ((unsigned char)ch >= 32 && (unsigned char)ch <= 126) {
                event.key = KEY_CHAR;
                event.ch = ch;
        }

        return event;
}