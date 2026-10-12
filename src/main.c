#include "gui/gui.h"
#include "relay/relay.h"
#include <string.h>

int main(int argc, char **argv) {
        if (argc == 3 && strcmp(argv[1], "--relay") == 0)  {
                return run_relay(argv[2]);
        }
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
