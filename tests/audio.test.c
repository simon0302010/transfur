#include "../src/interfaces/audio/audio.h"

int main(void) {
        struct audio_state state;
        struct chunk chunk;

        init_conn_audio(&state, "/dev/dsp");
        send_chunk_audio(&state, &chunk);

        return 0;
}