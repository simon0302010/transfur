#ifndef AUDIO_H
#define AUDIO_H

#include "../../misc/bool.h"
#include "../interfaces.h"

#define AUDIO_OK 0
#define AUDIO_ERR_ARG 1     /* NULL pointer or malformed chunk */
#define AUDIO_ERR_OPTIONS 2 /* not a valid options string */
#define AUDIO_ERR_STATE                                                        \
        3                 /* init_conn_bluetooth never succeded on this slot   \
                           */
#define AUDIO_ERR_FRAME 4 /* frame gave a length more than CHUNK_SIZE */
#define AUDIO_ERR_PLATFORM                                                     \
        5 /* this platform does not have a working implementation yet */
#define AUDIO_ERR_WRITE 6 /* write() call failed */
#define AUDIO_ERR_UNSUPPORTED                                                  \
        7                     /* The specified audio device is not supported */
#define AUDIO_ERR_NOT_FOUND 8 /* The specified audio device was not found */

struct audio_state {
        int fd_out;
        int sample_rate;
        tbool inited;
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
int close_conn_audio(void *conn);

/* It's obvious what this does */
const char *get_error_text_audio(int code);

#endif