#ifdef _WIN32
#include <windows.h>
#elif defined(__MSDOS__)
#include <dos.h>
#else
#include <time.h>
#endif

#include "transfur_time.h"

void sleep_ms(unsigned int ms) {
#ifdef _WIN32
        Sleep(ms);
#elif defined(__MSDOS__) || defined(__TURBOC__)
        delay(ms);
#else
        struct timespec ts;

        ts.tv_nsec = (ms % 1000) * 1000000L;
        ts.tv_sec = ms / 1000;

        nanosleep(&ts, NULL);
#endif
}

struct td get_unix_time(void) {
        struct td elapsed = {0, 0};

#ifdef _WIN32
        SYSTEMTIME st;

        GetSystemTime(&st);

        elapsed =
            get_unix_time_from_date(st.wYear, st.wMonth, st.wDay, st.wHour,
                                    st.wMinute, st.wSecond, st.wMilliseconds);
#elif defined(__MSDOS__) || defined(__TURBOC__)
        elapsed.seconds = time(NULL);
#else
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        elapsed.seconds = (time_sec_t)ts.tv_sec;
        elapsed.microseconds = (time_sec_t)ts.tv_nsec / 1000L;
#endif

        return elapsed;
}

char is_leap_year(short year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

struct td get_unix_time_from_date(short year, short month, short day,
                                  short hour, short minute, short second,
                                  short millisecond) {
        int i;
        short days_per_month[12] = {31, 28, 31, 30, 31, 30,
                                    31, 31, 30, 31, 30, 31};
        struct td elapsed = {0, 0};

        if (year < 1970)
                return elapsed;

        for (i = 1970; i < year; i++) {
                if (is_leap_year(i))
                        elapsed.seconds += 31622400L; /* Leap year */
                else
                        elapsed.seconds += 31536000L;
        }

        if (is_leap_year(year))
                days_per_month[1] = 29;

        for (i = 0; i < month - 1; i++) /* -1 to exclude current month */
                elapsed.seconds += (time_sec_t)days_per_month[i] * 86400L;

        elapsed.seconds += (time_sec_t)(day - 1) * 86400L;
        elapsed.seconds += (time_sec_t)hour * 3600;
        elapsed.seconds += (time_sec_t)minute * 60;
        elapsed.seconds += second;
        elapsed.microseconds += (time_sec_t)millisecond * 1000;

        return elapsed;
}