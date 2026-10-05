#include "gui/gui.h"

int main(void) {
        /* All app logic should now be written in `gui/app/` */


#ifdef USE_TUI
        run_tui("Transfur");
#elif defined(USE_BASIC_GUI)
        run_basic("Transfur", 800, 600); /* TODO: Make size dynamic */
#else
        if (run_gui("Transfur") != 0)
                perror("gui_init");
#endif

        return 0;
}
