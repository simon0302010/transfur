#ifndef MUSIC_H
#define MUSIC_H

#define MUSIC_DEFAULT_PATH "assets/bg.mp3"
#define MUSIC_DEFAULT_VOLUME 64

/*
Loop background music from path, does nothing if file is missing or audio device not present
*/

void basic_music_play(const char *path, int volume);

/* 
Stops and unloads current track
*/
void basic_music_stop(void);

/* Release audio device */
void basic_music_cleanup(void);

#endif