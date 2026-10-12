#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "../interfaces.h"

#define BT_OK 0
#define BT_ERR_ARG 1            /* NULL pointer or malformed chunk */
#define BT_ERR_OPTIONS 2        /* not a valid options string */
#define BT_ERR_STATE 3          /* init_conn_bluetooth never succeded on this slot */
#define BT_ERR_SOCKET 4         /* socket() failed */
#define BT_ERR_BIND 5           /* bind() failed */
#define BT_ERR_LISTEN 6         /* listen() failed */
#define BT_ERR_ACCEPT 7         /* accept() failed */
#define BT_ERR_CONNECT 8        /* connect() failed */
#define BT_ERR_SEND 9           /* send() failed */
#define BT_ERR_RECV 10          /* recv() failed */
#define BT_ERR_CLOSED 11        /* the peer hung up */
#define BT_ERR_FRAME 12         /* frame gave a length more than CHUNK_SIZE */
#define BT_ERR_ADDRESS 13       /* could not parse the bluetooth address */
#define BT_ERR_PLATFORM 14      /* this platform does not have a working implementation yet */

/*
Builds a two-way connection to another device over Bluetooth RFCOMM

The connection options are:
- "l:<channel>" >>> listen on channel on all local adapters
- "l:<bdaddr>:<channel>" >>> listen on one specific adapter
- "c:<bdaddr>:<channel>" >>> connect to peer
*/
int init_conn_bluetooth(void *conn, const char *options);

/*
Sends a chunk of data to the other device
*/
int send_chunk_bluetooth(void *conn, const struct chunk *chunk);

/*
Waits for a chunk on the receiver side, received data is written into chunk
*/
int recv_chunk_bluetooth(void *conn, struct chunk *chunk);

/*
Closes the connection's socket and resets the connection state
*/
int close_conn_bluetooth(void *conn);

const char *get_error_text_bluetooth(int code);

#endif