#ifdef USE_BASIC_GUI
#include "gui/tui/tui.h"
#else
#include "gui/gui.h"
#endif

int main(void) {
        /* All app logic should now be written in `gui/app/` */

#ifdef USE_TUI
        run_tui("Transfur");
#elif defined(USE_BASIC_GUI)
        /* run_basic_gui(); */
#else
        if (run_gui("Transfur") != 0)
                perror("gui_init");
#endif

        return 0;
}
