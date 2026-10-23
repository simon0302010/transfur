#ifndef AUDIO_H
#define AUDIO_H

#include "../../misc/bool.h"
#include "../interfaces.h"

#define AUDIO_OK 0
#define AUDIO_ERR_ARG 1     /* NULL pointer or malformed chunk */
#define AUDIO_ERR_OPTIONS 2 /* not a valid options string */
#define AUDIO_ERR_STATE                                                        \
        3 /* init_conn_audio never succeded on this slot or it's already taken \
           */
#define AUDIO_ERR_FRAME 4 /* frame gave a length more than CHUNK_SIZE */
#define AUDIO_ERR_PLATFORM                                                     \
        5 /* this platform does not have a working implementation yet */
#define AUDIO_ERR_BUSY 6  /* Audio device is already sending a chunk */
#define AUDIO_ERR_INIT 7  /* Failed to initialize device */
#define AUDIO_ERR_ALLOC 8 /* Failed to allocate memory */
#define AUDIO_ERR_PLAY                                                         \
        9 /* Unknown error occurred while trying to play sound                 \
           */

struct audio_state {
        char unused;
};

/*
Builds a two-way connection to another device over Audio.
`const char *options` points to a string containing the device id.
*/
int init_conn_audio(void *conn, const char *options);

/*
Sends a chunk of data to the other device
*/
int send_chunk_audio(void *conn, const struct chunk *chunk);

/*
Waits for a chunk on the receiver side, received data is written into chunk
*/
int recv_chunk_audio(void *conn, struct chunk *chunk);

/*
Closes the connection's socket and resets the connection state
*/
int close_conn_audio(void *conn, tbool is_last);

/* It's obvious what this does */
const char *get_error_text_audio(int code);

#endif