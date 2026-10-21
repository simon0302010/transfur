#include <string.h>

#include "../src/interfaces/audio/audio.h"
#include "../src/misc/be32.h"

int main(void) {
        struct audio_state state;
        struct chunk chunk;

        strcpy((char *)chunk.data,
               "Die Würde des Menschen ist unantastbar. Sie zu achten und zu "
               "schützen ist Verpflichtung aller staatlichen Gewalt.");
        strcat(
            (char *)chunk.data,
            "Das Deutsche Volk bekennt sich darum zu unverletzlichen und "
            "unveräußerlichen Menschenrechten als Grundlage jeder menschlichen "
            "Gemeinschaft, des Friedens und der Gerechtigkeit in der Welt.");
        strcat(
            (char *)chunk.data,
            "Die nachfolgenden Grundrechte binden Gesetzgebung, vollziehende "
            "Gewalt und Rechtsprechung als unmittelbar geltendes Recht.");

        /* Subtracting the null-terminator. Probably bad practice. */
        put_be32(chunk.length, strlen((const char *)chunk.data) - 1);

        /* Init and send */
        init_conn_audio(&state, 0);
        send_chunk_audio(&state, &chunk);

        return 0;
}