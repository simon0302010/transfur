#include "../core/ui_core.h"
#include "../core/widget.h"
#include "raw.h"
#include "../app/app.h"
#include "render.h"
#include "input.h"

int run_tui(const char *title) {
        struct ui_context ui;
        struct ui_event event;
        double current_time, prev_time, dt, elapsed;
        int timeout_ms;
        const double target_frame_duration = 1.0 / 30.0;

        ui_init(&ui);

        app_init(&ui);

        enable_raw_mode();

        while (ui.running) {
                current_time = get_unix_time(); /* TODO: make cross-platform */
                dt = current_time - prev_time;
                prev_time = current_time;

                ui_update(&ui, dt); /* TODO: add dt */

                reset_cursor();
                
                tui_render(&ui);

                elapsed = get_unix_time() - current_time;
                if (elapsed < target_frame_duration) {
                        timeout_ms = (int)((target_frame_duration - elapsed) * 1000.0);
                } else {
                        timeout_ms = 0;
                }

                event = tui_poll_key_event(timeout_ms);
                if (event.key != UI_KEY_NONE) { /* TODO: remove this, it doesn't support mouse events (although IG that doesn't matter in a TUI) */
                        ui_handle_event(&ui, &event);
                }
        }

        disable_raw_mode();
        return 0;
}
