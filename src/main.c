#include "gui/gui.h"
#include "relay/relay.h"
#include <stdio.h>
#include <string.h>

#define USE_BASIC_GUI

int main(int argc, char **argv) {
        if (argc == 3 && strcmp(argv[1], "--relay") == 0) {
#if defined(OS_WINDOWS) || defined(OS_LINUX)
                return run_relay(argv[2]);
#else
                fprintf(stderr, "Relay mode is unsupported on this platform\n");
                return 1;
#endif
        }

#ifdef USE_TUI
        if (run_tui("Transfur") != 0)
                perror("run_tui");
#elif defined(USE_BASIC_GUI)
        if (run_basic("Transfur") != 0)
                perror("run_basic");
#else
        if (run_gui("Transfur") != 0)
                perror("run_gui");
#endif

        return 0;
}
