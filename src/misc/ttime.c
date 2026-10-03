#ifdef OS_WINDOWS
#include <windows.h>
#elif defined(OS_MSDOS)
#include <dos.h>
#include <time.h>
#else
#include <time.h>
#endif

#include "ttime.h"

void sleep_ms(unsigned int ms) {
#ifdef OS_WINDOWS
        Sleep(ms);
#elif defined(OS_MSDOS)
        delay(ms);
#else
        struct timespec ts;

        ts.tv_nsec = (ms % 1000) * 1000000L;
        ts.tv_sec = ms / 1000;

        nanosleep(&ts, NULL);
#endif
}

double get_unix_time(void) {
#ifdef OS_WINDOWS
        SYSTEMTIME st;
        FILETIME ft;
        ULARGE_INTEGER uli;

        GetSystemTime(&st);
        SystemTimeToFileTime(&st, &ft);

        uli.LowPart = ft.dwLowDateTime;
        uli.HighPart = ft.dwHighDateTime;

        /* Windows FILETIME starts Jan 1, 1601 in 100-ns intervals.
           Subtract 11644473600 seconds to reach Unix Epoch (Jan 1, 1970). */
        return (double)(uli.QuadPart - 116444736000000000ULL) / 10000000.0;
#elif defined(OS_MSDOS)
        time_t now = time(NULL);
        clock_t ticks = clock();
        double subsecond =
            (double)(ticks % CLOCKS_PER_SEC) / (double)CLOCKS_PER_SEC;
        return (double)now + subsecond;
#else
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
#endif
}