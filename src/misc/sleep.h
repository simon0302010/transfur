#ifndef SLEEP_H
#define SLEEP_H

#ifdef _WIN32
#include <windows.h>
#elif defined(__MSDOS__)
#include <dos.h>
#else
#include <time.h>
#endif

void sleep_ms(unsigned int ms);

#endif