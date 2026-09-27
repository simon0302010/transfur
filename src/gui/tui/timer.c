#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif
#include <time.h>
#include "timer.h"

double get_time_seconds(void) {
        struct timespec ts;

        clock_gettime(CLOCK_MONOTONIC, &ts);

        return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}
