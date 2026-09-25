/*
Source file for Nuklear GUI code.
Needs to implement all functions defined in `gui.h`
*/

#include "../../interfaces/interfaces.h"
#include "../gui.h"

#include "../nuklear/nuklear.h"
#include <stdio.h>

#if defined(__linux__)
#include "linux/nuklear_linux.h"

#elif defined(_WIN32)
#include "windows/nuklear_windows.h"

#elif defined(__APPLE__)
#include "macos/nuklear_macos.h"

#else
#error "Unsupported platform"
#endif

static nk_size loading_bar_state = 0;
static int loading_bar_running = 0;

static enum interface receiver = if_empty;
static enum interface sender = if_empty;

static int receiver_connected = 0;
static int sender_connected = 0;

static void calculator(const char *title, struct nk_context *ctx, int width,
                       int height) {
        if (nk_begin(ctx, title, nk_rect(0, 0, width, height), 0)) {
                ctx->style.menu_button = ctx->style.button;

                nk_layout_row_dynamic(ctx, 20, 0);

                nk_layout_row_template_begin(ctx, 50);
                nk_layout_row_template_push_static(ctx, 180);
                nk_layout_row_template_push_dynamic(ctx);
                nk_layout_row_template_push_static(ctx, 180);
                nk_layout_row_template_push_dynamic(ctx);
                nk_layout_row_template_push_static(ctx, 180);
                nk_layout_row_template_end(ctx);

                /* Set color based on connection status */
                ctx->style.menu_button.normal = nk_style_item_color(
                    receiver_connected ? nk_rgba(0, 100, 0, 255)
                                       : nk_rgba(100, 0, 0, 255));
                ctx->style.menu_button.hover = nk_style_item_color(
                    receiver_connected ? nk_rgba(0, 80, 0, 255)
                                       : nk_rgba(80, 0, 0, 255));
                ctx->style.menu_button.active = nk_style_item_color(
                    receiver_connected ? nk_rgba(0, 60, 0, 255)
                                       : nk_rgba(60, 0, 0, 255));

                if (nk_menu_begin_label(ctx, get_receiver_text(receiver),
                                        NK_TEXT_CENTERED, nk_vec2(180, 120))) {
                        nk_layout_row_dynamic(ctx, 30, 1);
                        if (nk_menu_item_label(ctx, get_receiver_text(if_file),
                                               NK_TEXT_LEFT))
                                receiver = if_file;
                        if (nk_menu_item_label(ctx, get_receiver_text(if_lan),
                                               NK_TEXT_LEFT))
                                receiver = if_lan;
                        if (nk_menu_item_label(ctx,
                                               get_receiver_text(if_serial),
                                               NK_TEXT_LEFT))
                                receiver = if_serial;
                        nk_menu_end(ctx);
                }

                nk_spacer(ctx);

                if (nk_button_label(ctx, "Initialize connection")) {
                        loading_bar_running = !loading_bar_running;
                }

                nk_spacer(ctx);

                /* Set color based on connection status */
                ctx->style.menu_button.normal = nk_style_item_color(
                    sender_connected ? nk_rgba(0, 100, 0, 255)
                                     : nk_rgba(100, 0, 0, 255));
                ctx->style.menu_button.hover = nk_style_item_color(
                    sender_connected ? nk_rgba(0, 80, 0, 255)
                                     : nk_rgba(80, 0, 0, 255));
                ctx->style.menu_button.active = nk_style_item_color(
                    sender_connected ? nk_rgba(0, 60, 0, 255)
                                     : nk_rgba(60, 0, 0, 255));

                if (nk_menu_begin_label(ctx, get_sender_text(sender),
                                        NK_TEXT_CENTERED, nk_vec2(180, 120))) {
                        nk_layout_row_dynamic(ctx, 30, 1);
                        if (nk_menu_item_label(ctx, get_sender_text(if_file),
                                               NK_TEXT_LEFT))
                                sender = if_file;
                        if (nk_menu_item_label(ctx, get_sender_text(if_lan),
                                               NK_TEXT_LEFT))
                                sender = if_lan;
                        if (nk_menu_item_label(ctx, get_sender_text(if_serial),
                                               NK_TEXT_LEFT))
                                sender = if_serial;
                        nk_menu_end(ctx);
                }

                /* Vertical spacer */
                nk_layout_row_dynamic(ctx, 10, 1);

                nk_layout_row_dynamic(ctx, 20, 1);

                /* Loading bar */
                if (loading_bar_running) {
                        loading_bar_state += 5;
                        if (loading_bar_state > 100)
                                loading_bar_state = 0;
                        nk_progress(ctx, &loading_bar_state, 100, nk_false);
                }
        }
        nk_end(ctx);
}

int run_gui(const char *title) { return run_gui_nuklear(title, calculator); }