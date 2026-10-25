#include "audio.h"

#if defined(OS_LINUX) /* Supported systems */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../misc/be32.h"
#include "../../misc/bool.h"

#if defined(OS_LINUX)
#include <ao/ao.h>
#include <portaudio.h>
#endif

#define FREQUENCY 44100
#define BIT_DURATION 0.01f

#define FREQUENCY_FALSE 1100
#define FREQUENCY_TRUE 2200

/* Plays a tone and returns if it failed */
#define TONE(FREQ, T)                                                          \
        res = play_tone(FREQ, T);                                              \
        if (res != AUDIO_OK)                                                   \
                return res;

#if defined(OS_LINUX)
ao_device *sender_device;
ao_sample_format sender_format;
#endif
static tbool sender_busy = false;
static tbool sender_inited = false;
static float sender_phase = 0.0;

int init_conn_audio(void *conn, const char *options) {
        struct audio_state *state = (struct audio_state *)conn;

        if (!sender_inited) {
#if defined(OS_LINUX)
                int default_driver;

                ao_initialize();

                default_driver = ao_default_driver_id();

                memset(&sender_format, 0, sizeof(sender_format));
                sender_format.bits = 8;     /* Don't change */
                sender_format.channels = 1; /* Don't change */
                sender_format.rate = FREQUENCY;
                sender_format.byte_format = AO_FMT_LITTLE;

                sender_device = ao_open_live(default_driver, &sender_format,
                                             NULL /* no options */);
                if (sender_device == NULL) {
                        fprintf(stderr, "error opening audio device\n");
                        ao_shutdown();
                        return AUDIO_ERR_INIT;
                }
#endif
        }

        sender_inited = true;

        return AUDIO_OK;
}

/*
Writes an unterminated array of unsigned chars set to either 1 or 0 to DEST.
DEST must be at least 8 bytes large.
Reads from C in most signification byte first order.
A loop is not used to prevent unnecessary allocations and overhead on ancient
compilers.
*/
static void get_bits(unsigned char *dest, unsigned char c) {
        dest[0] = ((unsigned char)c >> 7) & 1u;
        dest[1] = ((unsigned char)c >> 6) & 1u;
        dest[2] = ((unsigned char)c >> 5) & 1u;
        dest[3] = ((unsigned char)c >> 4) & 1u;
        dest[4] = ((unsigned char)c >> 3) & 1u;
        dest[5] = ((unsigned char)c >> 2) & 1u;
        dest[6] = ((unsigned char)c >> 1) & 1u;
        dest[7] = ((unsigned char)c >> 0) & 1u;
}

static int play_tone(float hz, float duration) {
        unsigned long sample_count;
        unsigned long i;
        unsigned char *buffer;
        float phase_increment;

        sample_count = (unsigned long)(sender_format.rate * duration);
        buffer = calloc(sample_count, sizeof(unsigned char));
        if (buffer == NULL)
                return AUDIO_ERR_ALLOC;

        phase_increment = (hz * 2.0 * M_PI) / sender_format.rate;

        for (i = 0; i < sample_count; i++) {
                buffer[i] = (unsigned char)(128 + 127 * sin(sender_phase));

                sender_phase += phase_increment;
                if (sender_phase >= 2.0 * M_PI)
                        sender_phase -= 2.0 * M_PI;
        }

#if defined(OS_LINUX)
        if (!ao_play(sender_device, (char *)buffer, sample_count)) {
                free(buffer);
                return AUDIO_ERR_PLAY;
        }
#endif

        free(buffer);
        return AUDIO_OK;
}

int send_chunk_audio(void *conn, const struct chunk *chunk) {
        struct audio_state *state = (struct audio_state *)conn;
        int res;
        unsigned long i, b;
        unsigned long len;
        unsigned char bits[8];

        if (sender_busy)
                return AUDIO_ERR_BUSY;

        sender_busy = true;

        len = get_be32(chunk->length);
        if (len > CHUNK_SIZE) {
                sender_busy = false;
                return AUDIO_ERR_FRAME;
        }

        /* 100 tones to sync with receiver */
        for (i = 0; i < 50; i++) {
                TONE(FREQUENCY_TRUE, BIT_DURATION);
                TONE(FREQUENCY_FALSE, BIT_DURATION);
        }

        /* Chunk start */
        TONE(300, 0.3);

        /* Chunk type */
        get_bits(bits, chunk->type);
        for (i = 0; i < 8; i++) {
                TONE(bits[i] == 0 ? FREQUENCY_FALSE : FREQUENCY_TRUE,
                     BIT_DURATION);
        }

        /* Data length */
        for (i = 0; i < 4; i++) {
                get_bits(bits, chunk->length[i]);

                for (b = 0; b < 8; b++)
                        TONE(bits[b] == 0 ? FREQUENCY_FALSE : FREQUENCY_TRUE,
                             BIT_DURATION);
        }

        /* Data */
        for (i = 0; i < len; i++) {
                get_bits(bits, chunk->data[i]);

                for (b = 0; b < 8; b++)
                        TONE(bits[b] == 0 ? FREQUENCY_FALSE : FREQUENCY_TRUE,
                             BIT_DURATION);
        }

        /* Chunk end */
        TONE(300, 0.3);

        sender_busy = false;

        return AUDIO_OK;
}

int recv_chunk_audio(void *conn, struct chunk *chunk) { return AUDIO_OK; }

int close_conn_audio(void *conn, tbool is_last) {
        if (is_last) {
                sender_busy = false;
                sender_inited = false;
#if defined(OS_LINUX)
                ao_close(sender_device);
                sender_device = NULL;
                ao_shutdown();
#endif
        }

        return AUDIO_OK;
}

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

int close_conn_audio(void *conn, tbool is_last) { return AUDIO_ERR_PLATFORM; }

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
        case AUDIO_ERR_BUSY:
                return "The device is busy";
        case AUDIO_ERR_INIT:
                return "Failed to initialize the audio device";
        case AUDIO_ERR_ALLOC:
                return "Failed to allocate memory";
        case AUDIO_ERR_PLAY:
                return "An unknown error occurred while trying to play a sound";
        default:
                return "Unknown audio error";
        }
}
