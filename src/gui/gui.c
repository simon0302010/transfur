/*
This file decides what source file to include based on the operating system it
is being compiled for. The logic is worth improving when adding support for
embedded systems later on.
*/

#include "gui.h"

#if defined(__linux__)
#include "nuklear/gui_nuklear.c"

#elif defined(_WIN32)
#include "nuklear/gui_nuklear.c"

#elif defined(__APPLE__)
#include "nuklear/gui_nuklear.c"

#else
#error "Unsupported platform"
#endif