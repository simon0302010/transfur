#include "../../../platform.h"

#ifdef OS_WINDOWS

#ifndef NUKLEAR_WINDOWS_H
#define NUKLEAR_WINDOWS_H

#include "../nuklear.h"

int run_gui_nuklear(const char *title,
                    void (*gui)(const char *title, struct nk_context *ctx,
                                int width, int height));

#endif

#endif