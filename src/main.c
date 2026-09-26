#include <stdio.h>

#include "gui/gui.h"

#include "gui/tui/tui.h"

int main(void) {
        struct renderable my_content[2];
        struct progress_bar_options my_options;
        float progress = 0.54;
        char title[5] = "hEllo";

        my_content[0] = create_text(5, "Hello");

        my_options.progress = &progress;
        my_options.title = title;

        my_content[1] = create_progress_bar(&my_options, sizeof(my_options));

        render_content(my_content, 2);

        /*
        if (run_gui("Transfur") != 0)
                perror("gui_init");
        */

        return 0;
}
