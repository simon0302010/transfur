#include <stdio.h>

#include "gui/gui.h"

int main(void) {
        if (run_gui("Transfur") != 0)
                perror("gui_init");

        return 0;
}