#include "audio.h"

#if defined(OS_LINUX) /* Supported systems */

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
                return "Audio transfer is not yet supported on this system";
        default:
                return "Unknown audio error";
        }
}
