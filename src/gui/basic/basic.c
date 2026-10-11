#include "basic.h"
#include "../../misc/bool.h"
#include "../app/app.h"
#include "../core/ui_core.h"
#include "event.h"
#include "render.h"
#include "sdl3/basic_wrapper.h"
#include "theme.h"

void run_basic(char *title, int width, int height) {
        struct color color;
        struct ui_context ui;
        struct ui_event event;
        double prev_time, current_time, dt;

        color.r = 255;
        color.b = 0;
        color.g = 0;

        ui_init(&ui);

        app_init(&ui);

        prev_time = basic_get_time();

        basic_init(title, width, height);

        while (ui.running) {
                event = basic_poll_ui_event();
                switch (event.type) {
                case UI_EVENT_QUIT:
                        ui.running = false;
                        break;
                case UI_EVENT_KEY_DOWN:
                        if (event.key == UI_KEY_CHAR && event.ch == 't') {
                                /* TODO: make ui handle this first (so, e.g.,
                                 * you can type "t" in an input) */
                                basic_toggle_theme();
                                break;
                        }
                        ui_handle_event(&ui, &event);
                        break;
                case UI_EVENT_MOUSE_DOWN: {
                        struct widget *hit =
                            basic_get_widget_at(event.mouse_x, event.mouse_y);
                        ui_set_focus(&ui, hit);
                        ui_handle_event(&ui, &event);
                        break;
                }
                default:
                        ui_handle_event(&ui, &event);
                        break;
                }

                current_time = basic_get_time();
                dt = current_time - prev_time;
                prev_time = current_time;

                app_update(dt);
                ui_update(&ui, dt);

                basic_begin_frame();

                basic_render(&ui);

                basic_present();
        }

        basic_cleanup();

        return;
}
