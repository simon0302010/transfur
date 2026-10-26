#include "music.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h> 

static Mix_Music *s_music = NULL;
static int s_audio_open = 0;

static void music_free(void) {
        if (!s_music) {
                return;
        }

        Mix_HaltMusic();
        Mix_FreeMusic(s_music);
        s_music = NULL;
}

static int music_open(void) {
        int decoders;

        if (s_audio_open) {
                return 1;
        }

        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
                fprintf(stderr, "music: audio init failed %s\n", SDL_GetError());
                return 0;
        }
        
        decoders = Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
        if (!(decoders & MIX_INIT_MP3)) {
                fprintf(stderr, "music: mp3 decoder unavailable\n");
        }

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
                fprintf(stderr, "music: audio device failed %s\n", Mix_GetError());
                return 0;
        }

        s_audio_open = 1;
        return 1;
}

void basic_music_play(const char *path, int volume) {
        if (!path || !music_open()) {
                return;
        }

        music_free();

        s_music = Mix_LoadMUS(path);
        if (!s_music) {
                fprintf(stderr, "music: cannot load %s: %s", path, Mix_GetError());
                return;
        }

        if (volume < 0) {
                volume = 0;
        }
        if (volume > MIX_MAX_VOLUME) {
                volume = MIX_MAX_VOLUME;
        }

        Mix_VolumeMusic(volume);

        if (Mix_PlayMusic(s_music, -1) < 0) {
                fprintf(stderr, "music: cannot play %s: %s", path, Mix_GetError());
                music_free();
        }
}

void basic_music_stop(void) {
        music_free();
}

void basic_music_cleanup(void) {
        music_free();

        if (s_audio_open) {
                Mix_CloseAudio();
                s_audio_open = 0;
        }
        
        Mix_Quit();
}