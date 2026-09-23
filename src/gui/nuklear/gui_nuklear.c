/*
Source file for Nuklear GUI code.
Needs to implement all functions defined in `gui.h`
*/

#include <stdio.h>

#if defined(__linux__)
#include "linux/nuklear_linux.c"

#elif defined(_WIN32)
#include "windows_macos/nuklear_windows_macos.c"

#elif defined(__APPLE__)
#include "macos/nuklear_macos.c"

#else
#error "Unsupported platform"
#endif

static void calculator(const char *title, struct nk_context *ctx, int width,
                       int height) {
        if (nk_begin(ctx, title, nk_rect(0, 0, width, height), 0)) {
                static int set = 0, prev = 0, op = 0;
                static const char numbers[] = "789456123";
                static const char ops[] = "+-*/";
                static double a = 0, b = 0;
                static double *current = &a;

                size_t i = 0;
                int solve = 0;
                {
                        int len;
                        char buffer[256];
                        nk_layout_row_dynamic(ctx, 35, 1);
                        len = sprintf(buffer, "%.2f", *current);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, buffer, &len, 255,
                                       nk_filter_float);
                        buffer[len] = 0;
                        *current = atof(buffer);
                }

                nk_layout_row_dynamic(ctx, 35, 4);
                for (i = 0; i < 16; ++i) {
                        if (i >= 12 && i < 15) {
                                if (i > 12)
                                        continue;
                                if (nk_button_label(ctx, "C")) {
                                        a = b = op = 0;
                                        current = &a;
                                        set = 0;
                                }
                                if (nk_button_label(ctx, "0")) {
                                        *current = *current * 10.0f;
                                        set = 0;
                                }
                                if (nk_button_label(ctx, "=")) {
                                        solve = 1;
                                        prev = op;
                                        op = 0;
                                }
                        } else if (((i + 1) % 4)) {
                                if (nk_button_text(
                                        ctx, &numbers[(i / 4) * 3 + i % 4],
                                        1)) {
                                        *current =
                                            *current * 10.0f +
                                            numbers[(i / 4) * 3 + i % 4] - '0';
                                        set = 0;
                                }
                        } else if (nk_button_text(ctx, &ops[i / 4], 1)) {
                                if (!set) {
                                        if (current != &b) {
                                                current = &b;
                                        } else {
                                                prev = op;
                                                solve = 1;
                                        }
                                }
                                op = ops[i / 4];
                                set = 1;
                        }
                }
                if (solve) {
                        if (prev == '+')
                                a = a + b;
                        if (prev == '-')
                                a = a - b;
                        if (prev == '*')
                                a = a * b;
                        if (prev == '/')
                                a = a / b;
                        current = &a;
                        if (set)
                                current = &b;
                        b = 0;
                        set = 0;
                }
        }
        nk_end(ctx);
}

int run_gui(const char *title) { return run_gui_nuklear(title, calculator); }