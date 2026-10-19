#include "audio.h"

#if defined(OS_LINUX) /* Supported systems */

#include <stdio.h>
#include <stdlib.h>

#if defined(OS_LINUX)
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <unistd.h>
#endif

#define DEFAULT_SAMPLE_RATE 48000;

static int open_audio_device(int *fd, const char *name, int mode,
                             int *sample_rate) {
        int tmp;

        if ((*fd = open(name, mode, 0)) == -1) {
                perror(name);
                return AUDIO_ERR_NOT_FOUND;
        }

        tmp = AFMT_S16_NE; /* Native 16 bits */
        if (ioctl(*fd, SNDCTL_DSP_SETFMT, &tmp) == -1) {
                perror("SNDCTL_DSP_SETFMT");
                return AUDIO_ERR_UNSUPPORTED;
        }

        if (tmp != AFMT_S16_NE) {
                fprintf(stderr, "The audio device doesn't support the 16 bit "
                                "sample format.\n");
                return AUDIO_ERR_UNSUPPORTED;
        }

        tmp = 1;
        if (ioctl(*fd, SNDCTL_DSP_CHANNELS, &tmp) == -1) {
                perror("SNDCTL_DSP_CHANNELS");
                return AUDIO_ERR_UNSUPPORTED;
        }

        if (tmp != 1) {
                fprintf(stderr,
                        "The audio device doesn't support mono mode.\n");
                return AUDIO_ERR_UNSUPPORTED;
        }

        if (sample_rate == NULL)
                *sample_rate = DEFAULT_SAMPLE_RATE;

        if (ioctl(*fd, SNDCTL_DSP_SPEED, &sample_rate) == -1) {
                perror("SNDCTL_DSP_SPEED");
                return AUDIO_ERR_UNSUPPORTED;
        }

        return AUDIO_OK;
}

int init_conn_audio(void *conn, const char *options) {
        struct audio_state *state = (struct audio_state *)conn;

        if (state->inited == true)
                return AUDIO_ERR_STATE;

        state->sample_rate = DEFAULT_SAMPLE_RATE;
        open_audio_device(&state->fd_out, options, O_WRONLY,
                          &state->sample_rate);

        return AUDIO_OK;
}

int send_chunk_audio(void *conn, const struct chunk *chunk) {
        struct audio_state *state = (struct audio_state *)conn;
        static unsigned int phase = 0; /* Phase of the sine wave */
        unsigned int p;
        int i;
        short buf[1024]; /* 1024 samples/write is a safe choice */
        int outsz = sizeof(buf) / 2;

        static int sinebuf[48] = {
            0,      4276,   8480,   12539,  16383,  19947,  23169,  25995,
            28377,  30272,  31650,  32486,  32767,  32486,  31650,  30272,
            28377,  25995,  23169,  19947,  16383,  12539,  8480,   4276,
            0,      -4276,  -8480,  -12539, -16383, -19947, -23169, -25995,
            -28377, -30272, -31650, -32486, -32767, -32486, -31650, -30272,
            -28377, -25995, -23169, -19947, -16383, -12539, -8480,  -4276};

        if (state->inited == false)
                return AUDIO_ERR_STATE;

        for (i = 0; i < outsz; i++) {
                p = (phase * state->sample_rate) / 48000;

                phase = (phase + 1) % 4800;
                buf[i] = sinebuf[p % 48];
        }

        if (write(state->fd_out, buf, sizeof(buf)) != sizeof(buf)) {
                perror("audio_write");
                exit(-1);
        }

        return AUDIO_OK;
}

int recv_chunk_audio(void *conn, struct chunk *chunk) { return AUDIO_OK; }

int close_conn_audio(void *conn) { return AUDIO_OK; }

#else /* Unsupported systems */

int init_conn_audio(void *conn, const char *options) {
        return AUDIO_ERR_PLATFORM;
}

int send_chunk_audio(void *conn, const struct chunk *chunk) {
        return AUDIO_ERR_PLATFORM;
}

int recv_chunk_audio(void *conn, struct chunk *chunk) {
        return AUDIO_ERR_PLATFORM;
}

int close_conn_audio(void *conn) { return AUDIO_ERR_PLATFORM; }

#endif

const char *get_error_text_audio(int code) {
        switch (code) {
        case AUDIO_ERR_ARG:
                return "Bad argument";
        case AUDIO_ERR_OPTIONS:
                return "Invalid options (use <audio_device>)";
        case AUDIO_ERR_STATE:
                return "Audio slot already initialized";
        case AUDIO_ERR_FRAME:
                return "Received a malformed frame";
        case AUDIO_ERR_PLATFORM:
                return "Audio transfer is not yet supported on this "
                       "system";
        case AUDIO_ERR_WRITE:
                return "Failed to write to audio device";
        default:
                return "Unknown audio error";
        }
}
