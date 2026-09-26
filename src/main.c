#include <stdio.h>

#include "gui/gui.h"

#include "gui/tui/tui.h"

int main(void) {
        struct Renderable myContent[1];

        myContent[0] = createText(5, "Hello");

        renderContent(myContent, 1);

        /*
        if (run_gui("Transfur") != 0)
                perror("gui_init");
        */

        return 0;
}