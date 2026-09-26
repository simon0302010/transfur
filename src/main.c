#include <stdio.h>

#include "gui/gui.h"

#include "gui/tui/tui.h"

int main(void) {
        struct Renderable myContent[2];
        struct ProgressBarOptions myOptions;
        int progress = 50;
        char title[5] = "hEllo";

        myContent[0] = createText(5, "Hello");

        myOptions.progress = &progress;
        myOptions.title = title;

        myContent[1] = createProgressBar(&myOptions, sizeof(myOptions));

        renderContent(myContent, 2);

        /*
        if (run_gui("Transfur") != 0)
                perror("gui_init");
        */

        return 0;
}