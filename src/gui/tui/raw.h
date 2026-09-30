#if !defined(_WIN32) && !defined(__MSDOS__) && !defined(__TURBOC__)

#include <termios.h>

void disable_raw_mode(void);

void enable_raw_mode(void);

#endif