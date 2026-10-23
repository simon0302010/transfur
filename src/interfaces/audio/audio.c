#include "audio.h"

#if defined(OS_LINUX) /* Supported systems */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../misc/be32.h"
#include "../../misc/bool.h"

#if defined(OS_LINUX)
#include <SDL2/SDL.h> /* SDL2 was chosen because alsa doesn't work with C89 (I think) */
#endif

#define FREQUENCY 48000
#define BUFFER_LENGTH 1 /* 1/1s */
#define BUFFER_SAMPLES (FREQUENCY / BUFFER_LENGTH)

#define FREQUENCY_FALSE 1100
#define FREQUENCY_TRUE 2200

static tbool busy;

/* SDL only seems to expose one audio interface.
Opening it multiple times wouldn't make any sense. */
static SDL_AudioDeviceID dev;
static SDL_AudioSpec spec;
static Sint16 buffer[FREQUENCY / BUFFER_LENGTH];
static tbool inited;
static float phase = 0.0;

static Sint16 format(float sample, float *amplitude) {
        return (Sint16)(sample * 32567 * *amplitude);
}

static int play_tone(SDL_AudioDeviceID *dev, Sint16 *buffer, float hz,
                     float amplitude, float duration) {
        unsigned long i;
        unsigned long samples = FREQUENCY * duration;
        float phase_increment = (hz * 2.0 * M_PI) / FREQUENCY;

        for (i = 0; i < samples; i++) {
                float sample_val = sin(phase);

                buffer[i] = format(sample_val, &amplitude);

                phase += phase_increment;

                if (phase >= 2.0 * M_PI)
                        phase -= 2.0 * M_PI;
        }

        if (SDL_QueueAudio(*dev, buffer, samples * sizeof(buffer[0])) < 0) {
                fprintf(stderr, "failed to queue audio: %s\n", SDL_GetError());
                return AUDIO_ERR_QUEUE;
        }

        return AUDIO_OK;
}

int init_conn_audio(void *conn, const char *options) {
        struct audio_state *state = (struct audio_state *)conn;

        if (inited == true)
                return AUDIO_ERR_BUSY;

        spec.freq = FREQUENCY;
        spec.format = AUDIO_S16SYS;
        spec.channels = 1;
        spec.callback = NULL;
        spec.samples = 4096;
        spec.userdata = NULL;

        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
                fprintf(stderr, "SDL failed to initialize : %s\n",
                        SDL_GetError());
                return AUDIO_ERR_INIT;
        }

        dev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
        if (dev == 0) {
                fprintf(stderr, "failed to open audio: %s\n", SDL_GetError());
                SDL_Quit();
                return AUDIO_ERR_INIT;
        }

        inited = true;

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

int send_chunk_audio(void *conn, const struct chunk *chunk) {
        struct audio_state *state = (struct audio_state *)conn;
        int res;
        unsigned long i, b;
        unsigned long len;
        unsigned char bits[8];

        if (busy)
                return AUDIO_ERR_BUSY;

        busy = true;

        len = get_be32(chunk->length);
        if (len > CHUNK_SIZE)
                return AUDIO_ERR_FRAME;

        /* Indicates chunk start */
        res = play_tone(&dev, buffer, 300, 0.5, 0.3);
        if (res != AUDIO_OK)
                return res;

        /* Chunk type */
        res = play_tone(&dev, buffer, chunk->type * 25 + 500, 0.5, 0.3);
        if (res != AUDIO_OK)
                return res;

        /* Data start */
        res = play_tone(&dev, buffer, 400, 0.5, 0.3);
        if (res != AUDIO_OK)
                return res;

        /* Data */
        for (i = 0; i < len; i++) {
                get_bits(bits, chunk->data[i]);

                for (b = 0; b < 8; b++) {
                        res = play_tone(&dev, buffer,
                                        bits[b] == 0 ? FREQUENCY_FALSE
                                                     : FREQUENCY_TRUE,
                                        0.5, 0.01);
                        if (res != AUDIO_OK)
                                return res;
                }
        }

        /* Data end */
        res = play_tone(&dev, buffer, 400, 0.5, 0.3);
        if (res != AUDIO_OK)
                return res;

        /* Chunk end */
        res = play_tone(&dev, buffer, 300, 0.5, 0.3);
        if (res != AUDIO_OK)
                return res;

        SDL_PauseAudioDevice(dev, 0);

        while (SDL_GetQueuedAudioSize(dev) > 0) {
                SDL_Delay(10);
        }

        busy = false;

        return AUDIO_OK;
}

int recv_chunk_audio(void *conn, struct chunk *chunk) { return AUDIO_OK; }

int close_conn_audio(void *conn) {
        SDL_CloseAudioDevice(dev);
        SDL_Quit();

        inited = false;
        dev = 0;

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
        case AUDIO_ERR_BUSY:
                return "The device is busy";
        case AUDIO_ERR_INIT:
                return "Failed to initialize the audio device";
        case AUDIO_ERR_QUEUE:
                return "Failed to queue the next tone";
        default:
                return "Unknown audio error";
        }
}
