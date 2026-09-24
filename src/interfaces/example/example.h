#ifndef EXAMPLE_H
#define EXAMPLE_H

#include "../interfaces.h"

/*
`void *options` points to a set of options for the interface (ip address, serial
port, etc.).
The implementing individual can decide what to use `options` for.
This must be called once per connection and intelligently build a
two-way connection. `void *conn` is a pointer pointing to 512 bytes of zeroed
memory which must be used to save the connection state.
*/
int init_conn_example(void *conn, void *options);

/*
Sends a chunk of data to the interface.
`recv_chunk` must be running on the receiver end to receive the data.
Returns `0` if the chunk transfer succeeded or any other code based on the
error.
*/
int send_chunk_example(void *conn, const struct chunk *chunk);

/*
Waits for a chunk on the receiver side.
Received data is written into `chunk`.
Returns `0` if the chunk transfer succeeded or any other code based on the
error.
*/
int recv_chunk_example(void *conn, struct chunk *chunk);

#endif