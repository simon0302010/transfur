#ifdef __linux__

#ifndef NUKLEAR_LINUX_H
#define NUKLEAR_LINUX_H

#include "../nuklear.h"

int run_gui_nuklear(const char *title,
                    void (*gui)(const char *title, struct nk_context *ctx,
                                int width, int height));

#endif

#endif