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
#define BUFFER_LENGTH 30 /* 1/30s */
#define BUFFER_SAMPLES (FREQUENCY / BUFFER_LENGTH)

static tbool busy;

/* SDL only seems to expose one audio interface.
Opening it multiple times wouldn't make any sense. */
static SDL_AudioDeviceID dev;
static SDL_AudioSpec spec;
static Sint16 buffer[FREQUENCY / BUFFER_LENGTH];
static tbool inited;

static Sint16 format(double sample, double *amplitude) {
        return (Sint16)(sample * 32567 * *amplitude);
}

static double tone(double *hz, unsigned long *time) {
        return sin(*time * *hz * M_PI * 2 / FREQUENCY);
}

static double saw(double *hz, unsigned long *time) {
        return fmod(*time * *hz / FREQUENCY, 1) * 2 - 1;
}

static double square(double *hz, unsigned long *time) {
        double sine = tone(hz, time);
        return sine > 0.0 ? 1.0 : -1.0;
}

static int play_tone(SDL_AudioDeviceID *dev, Sint16 *buffer, double hz,
                     double amplitude) {
        unsigned long i;

        for (i = 0; i < BUFFER_SAMPLES; i++) {
                buffer[i] = format(tone(&hz, &i), &amplitude);
        }

        if (SDL_QueueAudio(*dev, buffer, BUFFER_SAMPLES * sizeof(buffer[0])) <
            0) {
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

int send_chunk_audio(void *conn, const struct chunk *chunk) {
        struct audio_state *state = (struct audio_state *)conn;
        int res;
        unsigned long i;
        unsigned long len;

        if (busy)
                return AUDIO_ERR_BUSY;

        busy = true;

        len = get_be32(chunk->length);
        if (len > CHUNK_SIZE)
                return AUDIO_ERR_FRAME;

        for (i = 0; i < len; i++) {
                res = play_tone(&dev, buffer, chunk->data[i] * 25 + 500, 0.5);
                if (res != AUDIO_OK)
                        return res;
        }

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
