#include "basic.h"
#include "../../misc/bool.h"
#include "../../misc/ttime.h"
#include "../app/app.h"
#include "../core/ui_core.h"
#include "../gui.h"
#include "event.h"
#include "render.h"
#include "theme.h"
#include "theme_switcher.h"
#include "wrappers/basic_wrapper.h"

int run_basic(char *title) {
        struct color color;
        struct ui_context ui;
        struct ui_event event;
        double prev_time, current_time, dt, remaining_time;
        double time_per_frame = 1.0 / 30.0;

        color.r = 255;
        color.b = 0;
        color.g = 0;

        ui_init(&ui);

        app_init(&ui);

        prev_time = basic_get_time();

        basic_init(title, WINDOW_WIDTH, WINDOW_HEIGHT);

        while (ui.running) {
                while ((event = basic_poll_ui_event()).type != UI_EVENT_NONE) {
                        switch (event.type) {
                        case UI_EVENT_QUIT:
                                ui.running = false;
                                break;
                        case UI_EVENT_MOUSE_DOWN: {
                                struct widget *hit = basic_get_widget_at(
                                    event.mouse_x, event.mouse_y);
                                ui_set_focus(&ui, hit);
                                ui_handle_event(&ui, &event);
                                break;
                        }
                        default:
                                if (!ui_handle_event(&ui, &event)) {
                                        if (event.type == UI_EVENT_KEY_DOWN &&
                                            event.key == UI_KEY_CHAR &&
                                            event.ch == 't') {
                                                basic_toggle_theme();
                                                theme_switcher_show();
                                        }
                                }
                                break;
                        }
                }

                current_time = basic_get_time();
                dt = current_time - prev_time;
                prev_time = current_time;

                app_update(dt);
                ui_update(&ui, dt);

                theme_switcher_update(dt);

                basic_begin_frame();

                basic_render(&ui);

                theme_switcher_render(WINDOW_WIDTH, WINDOW_HEIGHT);

                basic_present();

                remaining_time =
                    time_per_frame - (basic_get_time() - prev_time);

                if (remaining_time > 0)
                        sleep_ms((unsigned int)(remaining_time * 1000));
        }

        basic_cleanup();

        return 0;
}
