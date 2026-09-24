#ifndef LAN_H
#define LAN_H

#include "../interfaces.h"

/*
`const char *ip` points to null-terminated string containing the other devices
ip. This must be called once per connection and intelligently build a two-way
connection. `void *conn` is a pointer pointing to 512 bytes of zeroed memory
which must be used to save the connection state.
*/
int init_conn_lan(void *conn, const char *ip);

/*
Sends a chunk of data to the other device.
`recv_chunk` must be running on the receiver end to receive the data.
Returns `0` if the chunk transfer succeeded or any other code based on the
error.
*/
int send_chunk_lan(void *conn, const struct chunk *chunk);

/*
Waits for a chunk on the receiver side.
Received data is written into `chunk`.
Returns `0` if the chunk transfer succeeded or any other code based on the
error.
*/
int recv_chunk_lan(void *conn, struct chunk *chunk);

#endif