#include <stdio.h>

#include "gui/gui.h"

int main(void) {
        if (create_window("Transfur") != 0)
                perror("gui_init");

        return 0;
}