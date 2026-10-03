#define USE_BASIC_GUI

#ifdef USE_BASIC_GUI
#include "gui/tui/tui.h"
#else
#include "gui/gui.h"
#endif

void on_btn_click(void) { /* Nothing for now*/ }

int main(void) {
        /* struct basic_renderable renderables[1]; */

        /* TODO: make this have renderables */
        /* run_basic("Transfur (Basic GUI Test)", 1080, 720, renderables, 0); */

        /* TODO: merge TUI and basic GUI renderables into a single thing
         * suppored by both */

#ifdef USE_BASIC_GUI
        struct renderable my_content[5];
        struct progress_bar_options my_options;
        struct loading_bar_options loading_bar_options;
        struct text_input_options text_input_options;
        struct button_options button_options;
        float progress = 0.54;
        char title[] = "hEllo";
        char text_buffer[50] = "";

        my_content[0] = create_text(5, "Hello");

        my_options.progress = &progress;
        my_options.title = title;

        my_content[1] = create_progress_bar(&my_options, sizeof(my_options));

        loading_bar_options.title = "Loading...";
        loading_bar_options.offset = 0;
        loading_bar_options.speed = 6.0f;
        loading_bar_options.accumulator = 0.0f;

        my_content[2] = create_loading_bar(&loading_bar_options,
                                           sizeof(loading_bar_options));

        text_input_options.buffer = text_buffer;
        text_input_options.cursor = 0;
        text_input_options.is_focused = false;
        text_input_options.label = title;
        text_input_options.max_len = 50;

        my_content[3] =
            create_text_input(&text_input_options, sizeof(text_input_options));

        button_options.label = "btn";
        button_options.is_focused = false;
        button_options.on_click = on_btn_click;

        my_content[4] = create_button(&button_options, sizeof(button_options));

        run_tui(my_content, 5);
#else
        if (run_gui("Transfur") != 0)
                perror("gui_init");
#endif

        return 0;
}
