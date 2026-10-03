#if !defined(OS_WINDOWS) && !defined(OS_MSDOS)

#include <termios.h>

void disable_raw_mode(void);

void enable_raw_mode(void);

void reset_cursor(void);

#endif