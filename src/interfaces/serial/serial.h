#ifndef SERIAL_H
#define SERIAL_H

/* Probably gonna have to change this */
#define BAUD_RATE 38400

#include "../interfaces.h"

#define SERIAL_OK 0
#define SERIAL_ERR_ARG 1     /* NULL pointer or malformed chunk */
#define SERIAL_ERR_OPTIONS 2 /* not a valid options string */
#define SERIAL_ERR_STATE 3   /* init_conn_serial never succeded on this slot */
#define SERIAL_ERR_OPEN 4    /* open() failed */
#define SERIAL_ERR_CONFIG 5  /* configuring the port failed */
#define SERIAL_ERR_SEND 6    /* write() failed */
#define SERIAL_ERR_RECV 7    /* read() failed */
#define SERIAL_ERR_CLOSED 8  /* the peer hung up ):< */
#define SERIAL_ERR_FRAME 9   /* frame gave a length more than CHUNK_SIZE */
#define SERIAL_ERR_PLATFORM                                                    \
        10 /* this platform does not have a working implementation yet */

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