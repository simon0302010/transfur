#ifndef PLATFORM_H
#define PLATFORM_H

#if defined(__linux__)
#define OS_LINUX
#elif defined(_WIN32)
#define OS_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
#define OS_MACOS
#elif defined(__MSDOS__) || defined(__TURBOC__) || defined(__BORLANDC__)
#define OS_MSDOS
#else
#define OS_UNKNOWN
#endif

#endif