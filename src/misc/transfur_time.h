#ifndef TIME_H
#define TIME_H

#if defined(__MSDOS__) || defined(__TURBOC__)
typedef long time_sec_t;
#else
#include <stdint.h>
typedef int64_t time_sec_t;
#endif

struct td {
        time_sec_t seconds;
        time_sec_t microseconds;
};

void sleep_ms(unsigned int ms);

#endif