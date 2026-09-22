/*
This file decides what source file to include based on the operating system it is being compiled for.
The logic is worth improving when adding support for embedded systems later on.
*/

#include "gui.h"

#if defined(__linux__)
#include "linux/gui_linux.c"

#elif defined(_WIN32)
#include "gui_windows.c"

#elif defined(__APPLE__)
#include "gui_macos.c"

#else
#error "Unsupported platform"
#endif