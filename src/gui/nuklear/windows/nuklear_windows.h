#ifndef NUKLEAR_WINDOWS_H
#define NUKLEAR_WINDOWS_H

#ifdef _WIN32

#include "../nuklear.h"

int run_gui_nuklear(const char *title,
                    void (*gui)(const char *title, struct nk_context *ctx,
                                int width, int height));

#endif

#endif