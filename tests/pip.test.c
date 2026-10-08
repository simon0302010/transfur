#include "../src/interfaces/lan/pip.h"
#include <stdio.h>

int main(void) {
        char buf[16];
        int res;

        res = get_public_ip(buf, sizeof(buf));

        printf("%s\n", buf);

        return res;
}