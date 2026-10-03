#include "basic.h"
#include "../../misc/bool.h"
#include "../app/app.h"
#include "../core/ui_core.h"
#include "event.h"
#include "render.h"
#include "text.h"

void run_basic(char *title, int width, int height) {
        struct color color;
        struct ui_context ui;
        struct ui_event event;

        color.r = 255;
        color.b = 0;
        color.g = 0;

        ui_init(&ui);

        app_init(&ui);

        basic_init(title, width, height);

        while (ui.running) {
                event = basic_poll_ui_event();
                switch (event.type) {
                case UI_EVENT_QUIT:
                        ui.running = false;
                        break;
                default:
                        break;
                }

                basic_begin_frame();

                basic_render(&ui);

                basic_present();
        }

        basic_cleanup();

        return;
}
