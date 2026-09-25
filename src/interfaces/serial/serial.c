/*
Placeholder file for now.
The goal is to have multiple "interfaces" which allow the application to
transfer data in various ways. Each interface is supposed to provide the same
set of functions for sending a receiving data which is defined in
`../interfaces/interfaces.h`. Using those functions, the user will be able to
build a chain of devices to transfer data. The code in this file along with its
counterparts for other ways to transfer data are expected to work on any
operating system. If this cannnot be guaranteed, it must be clearly stated in
the README and not compiled for systems with lacking support.
*/

#include "../interfaces.h"

int init_conn_serial(void *conn, const char *port) { return 0; }

int send_chunk_serial(void *conn, const struct chunk *chunk) { return 0; }

int recv_chunk_serial(void *conn, struct chunk *chunk) { return 0; }
