#include "SDL_audio.h"
#include "SDL_error.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

#define FREQUENCY 48000
#define BUFFER_LENGTH 10 /* 1/100s */
#define BUFFER_SAMPLES (FREQUENCY / BUFFER_LENGTH)

void play_buffer(void *, unsigned char *, int);

SDL_AudioSpec spec;

int buffer_pos = 0;

Sint16 format(double sample, double *amplitude) {
        return (Sint16)(sample * 32567 * *amplitude);
}

double tone(double *hz, unsigned long *time) {
        return sin(*time * *hz * M_PI * 2 / FREQUENCY);
}

double saw(double *hz, unsigned long *time) {
        return fmod(*time * *hz / FREQUENCY, 1) * 2 - 1;
}

double square(double *hz, unsigned long *time) {
        double sine = tone(hz, time);
        return sine > 0.0 ? 1.0 : -1.0;
}

void play_tone(SDL_AudioDeviceID *dev, Sint16 *buffer, double hz,
               double amplitude) {
        unsigned long i;

        for (i = 0; i < BUFFER_SAMPLES; i++) {
                buffer[i] = format(tone(&hz, &i), &amplitude);
        }

        if (SDL_QueueAudio(*dev, buffer, BUFFER_SAMPLES * sizeof(buffer[0])) <
            0) {
                fprintf(stderr, "failed to queue audio: %s\n", SDL_GetError());
        }
}

int main(int argc, char *argv[]) {
        SDL_AudioDeviceID dev;
        Sint16 buffer[FREQUENCY / BUFFER_LENGTH];
        int i;
        const char *data =
            "Die Würde des Menschen ist unantastbar. Sie zu achten und zu "
            "schützen ist Verpflichtung aller staatlichen Gewalt.";

        spec.freq = FREQUENCY;
        spec.format = AUDIO_S16SYS;
        spec.channels = 1;
        spec.callback = NULL;
        spec.samples = 4096;
        spec.userdata = NULL;

        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
                printf("SDL failed to initialize : %s\n", SDL_GetError());
                return -1;
        }

        dev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
        if (dev == 0) {
                fprintf(stderr, "failed to open audio: %s\n", SDL_GetError());
                SDL_Quit();
                return -1;
        }

        for (i = 0; i < strlen(data); i++) {
                play_tone(&dev, buffer, data[i] * 25 + 500, 0.5);
        }

        SDL_PauseAudioDevice(dev, 0);

        while (SDL_GetQueuedAudioSize(dev) > 0) {
                SDL_Delay(10);
        }

        SDL_CloseAudioDevice(dev);
        SDL_Quit();

        return 0;
}