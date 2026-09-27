
/* #include "gui/gui.h" */

#include "gui/tui/tui.h"
#include "misc/sleep.h"

int main(void) {
        struct renderable my_content[3];
        struct progress_bar_options my_options;
        struct loading_bar_options loading_bar_options;
        float progress = 0.54;
        char title[5] = "hEllo";

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

        run_tui(my_content, 3);

        /*
        if (run_gui("Transfur") != 0)
                perror("gui_init");
        */

        return 0;
}
