#ifndef WAN_H
#define WAN_H

#include "../interfaces.h"

#define WAN_OK 0
#define WAN_ERR_ARG 1     /* NULL pointer or malformed chunk */
#define WAN_ERR_OPTIONS 2 /* not a valid options string */
#define WAN_ERR_STATE 3   /* init_conn_wan never succeded on this slot */
#define WAN_ERR_SOCKET 4  /* socket() failed */
#define WAN_ERR_RESOLVE 5 /* could not resolve the relay host */
#define WAN_ERR_CONNECT 6 /* could not reach the relay */
#define WAN_ERR_RELAY 7   /* relay refused handshake or gave up */
#define WAN_ERR_SEND 8    /* send() failed */
#define WAN_ERR_RECV 9    /* recv() failed */
#define WAN_ERR_CLOSED 10 /* the peer (or relay) hung up */
#define WAN_ERR_FRAME 11  /* frame gave a length more than CHUNK_SIZE */
#define WAN_ERR_PLATFORM                                                       \
        12 /* this platform does not have a working implementation yet */

/*
Builds a two-way connection to another device via a relay server
*/
int init_conn_wan(void *conn, const char *options);

/*
Sends a chunk through the relay to the other device
*/
int send_chunk_wan(void *conn, const struct chunk *chunk);

/*
Waits for a chunk comming in through the relay
*/
int recv_chunk_wan(void *conn, struct chunk *chunk);

/*
Closes the relay connection and resets the connection state
*/
int close_conn_lan(void *conn);

const char *get_error_text_wan(int code);

#endif