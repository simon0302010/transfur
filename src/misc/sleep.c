#ifdef _WIN32
#include <windows.h>
#elif defined(__MSDOS__)
#include <dos.h>
#else
#include <time.h>
#endif

#include "sleep.h"

void sleep_ms(unsigned int ms) {
#ifdef _WIN32
        Sleep(ms);
#elif defined(__MSDOS__)
        delay(ms);
#else
        struct timespec ts;

        ts.tv_nsec = (ms % 1000) * 1000000L;
        ts.tv_sec = ms / 1000;

        nanosleep(&ts, NULL);
#endif
}