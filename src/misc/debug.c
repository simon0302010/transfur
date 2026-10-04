#ifdef TRANSFUR_DEBUG
#include <stdarg.h>
#include <stdio.h>
#endif

void print_debug(const char *format, ...) {
#ifdef TRANSFUR_DEBUG
        va_list args;

        va_start(args, format);
        vprintf(format, args);
        va_end(args);
#endif
        return;
}