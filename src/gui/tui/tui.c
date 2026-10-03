#include "../core/ui_core.h"
#include "../core/widget.h"
#include "raw.h"
#include "../app/app.h"

int run_tui(const char *title) {
        struct ui_context ui;
        /* struct ui_event event; */
        int timeout_ms;

        ui_init(&ui);

        app_init(&ui);

        enable_raw_mode();

        while (ui.running) {
                /* TODO: handle timing */

                /* TODO: handle events */

                ui_update(&ui, 0); /* TODO: add dt */

                reset_cursor();
                /* TODO: render widgets */
        }

        disable_raw_mode();
        return 0;
}
