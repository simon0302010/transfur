#ifdef __APPLE__

#ifndef NUKLEAR_MACOS_H
#define NUKLEAR_MACOS_H

#include "../nuklear.h"

int run_gui_nuklear(const char *title,
                    void (*gui)(const char *title, struct nk_context *ctx,
                                int width, int height));

#endif

#endif