#ifndef SERIAL_H
#define SERIAL_H

/* Probably gonna have to change this */
#define BAUD_RATE 38400

#include "../interfaces.h"

/*
`const char *port` points to a null-terminated string containing the serial port
name (on windows) or the path (on linux). This must be called once per
connection and intelligently build a two-way connection. `void *conn` is a
pointer pointing to 512 bytes of zeroed memory which must be used to save the
connection state.
*/
int init_conn_serial(void *conn, const char *port);

/*
Sends a chunk of data to the serial port.
`recv_chunk` must be running on the receiver end to receive the data.
Returns `0` if the chunk transfer succeeded or any other code based on the
error.
*/
int send_chunk_serial(void *conn, const struct chunk *chunk);

/*
Waits for a chunk on the receiver side.
Received data is written into `chunk`.
Returns `0` if the chunk transfer succeeded or any other code based on the
error.
*/
int recv_chunk_serial(void *conn, struct chunk *chunk);

#endif