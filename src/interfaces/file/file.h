#ifndef FILE_H
#define FILE_H

#include "../interfaces.h"

/*
This must be called once per connection and intelligently build a
two-way connection. `void *conn` is a pointer pointing to 512 bytes of zeroed
memory which must be used to save the connection state.
*/
int init_conn_file(void *conn, const char *file_path);

/*
Sends a chunk of data to the file.
`recv_chunk` must be running on the receiver end to receive the data.
Returns `0` if the chunk transfer succeeded or any other code based on the
error.
*/
int send_chunk_file(void *conn, const struct chunk *chunk);

/*
Reads a chunk from the file.
Received data is written into `chunk`.
Returns `0` if the chunk transfer succeeded or any other code based on the
error.
*/
int recv_chunk_file(void *conn, struct chunk *chunk);

#endif