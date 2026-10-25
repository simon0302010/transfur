#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../src/interfaces/audio/audio.h"
#include "../src/misc/be32.h"

int main(void) {
        struct audio_state state;
        struct chunk chunk_send;
        struct chunk chunk_recv;
        int result;

        strcpy((char *)chunk_send.data,
               "Die Würde des Menschen ist unantastbar. Sie zu achten und zu "
               "schützen ist Verpflichtung aller staatlichen Gewalt.");
        strcat(
            (char *)chunk_send.data,
            "Das Deutsche Volk bekennt sich darum zu unverletzlichen und "
            "unveräußerlichen Menschenrechten als Grundlage jeder menschlichen "
            "Gemeinschaft, des Friedens und der Gerechtigkeit in der Welt.");
        strcat(
            (char *)chunk_send.data,
            "Die nachfolgenden Grundrechte binden Gesetzgebung, vollziehende "
            "Gewalt und Rechtsprechung als unmittelbar geltendes Recht.");

        /* Subtracting the null-terminator. Probably bad practice. */
        put_be32(chunk_send.length, strlen((const char *)chunk_send.data) - 1);

        chunk_send.type = chunk_type_data;

        /* Init and send */
        result = init_conn_audio(&state, NULL);
        if (result != AUDIO_OK) {
                fprintf(stderr, "audio init failed: %s (%d)\n",
                        get_error_text_audio(result), result);
                return result;
        }

        /* result = send_chunk_audio(&state, &chunk);
        if (result != AUDIO_OK) {
                close_conn_audio(&state, true);
                return result;
        } */

        result = recv_chunk_audio(&state, &chunk_send);

        sleep(5);

        close_conn_audio(&state, true);

        return result;
}