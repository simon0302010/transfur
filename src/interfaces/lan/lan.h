#ifndef LAN_H
#define LAN_H

#include "../interfaces.h"

#define LAN_OK 0
#define LAN_ERR_ARG 1           /* NULL pointer or malformed chunk */
#define LAN_ERR_OPTIONS 2      /* not a valid options string */
#define LAN_ERR_STATE 3         /* init_conn_lan never succeeded on this slot */
#define LAN_ERR_SOCKET 4        /* socket() failed */
#define LAN_ERR_BIND 5          /* bind() filed */
#define LAN_ERR_LISTEN 6        /* listen() failed */
#define LAN_ERR_ACCEPT 7        /* accept() failed */
#define LAN_ERR_CONNECT 8       /* connect() failed */
#define LAN_ERR_SEND 9          /* send() failed */
#define LAN_ERR_RECV 10         /* recv() failed */
#define LAN_ERR_CLOSED 11       /* the peer hung up ):< */
#define LAN_ERR_FRAME 12        /* frame gave a length more than CHUNK_SIZE */
#define LAN_ERR_PLATFORM 13     /* this platform does not have a working implementation yet */

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