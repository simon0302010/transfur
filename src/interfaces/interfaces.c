#include <stddef.h>

#include "interfaces.h"

static unsigned char connections[MAX_CONN][512];
static enum interface connection_types[MAX_CONN];

/*
Writes the first empty connection slot index into `int *conn`
Returns `NULL` if no more slots are available.
Otherwise, a pointer to 512 bytes of memory will be returned for the interface
to write its data into.
*/
void *get_new_conn(enum interface if_type, int *conn) {
        int i;

        for (i = 0; i < MAX_CONN; i++) {
                if (connection_types[i] == if_empty) {
                        *conn = i;
                        connection_types[i] = if_type;
                        return connections[i];
                }
        }

        return NULL;
}