#if !defined(_WIN32) && !defined(__MSDOS__)

#include <termios.h>

void disable_raw_mode(void);

void enable_raw_mode(void);

#endif