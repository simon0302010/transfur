#include "../src/interfaces/lan/pip.h"
#include <stdio.h>

int main(void) {
        char buf[16];
        int res;

        printf("%s\n", get_public_ip()); /* Could be NULL but idc */

        return res;
}