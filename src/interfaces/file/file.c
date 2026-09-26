/*
Placeholder file for now.
The goal is to have multiple "interfaces" which allow the application to
transfer data in various ways. Each interface is supposed to provide the same
set of functions for sending a receiving data which is defined in
`../interfaces/interfaces.h`. Using those functions, the user will be able to
build a chain of devices to transfer data. The code in this file along with its
counterparts for other ways to transfer data are expected to work on any
operating system. If this cannnot be guaranteed, it must be clearly stated in
the README and not compiled for systems with lacking support.

The receive function of the `file` interface reads from a file while the send
function writes to a file. Only the receiving file interface is meant to process
chunk types 1 and 2.
*/
#include <stdio.h>
#include <string.h>

#include "../interfaces.h"

/* File path limit */
#define FILE_PATH_LIMIT 480

struct file_state {
        unsigned long offset;
        char path[FILE_PATH_LIMIT];
};

typedef char file_state_fits[sizeof(struct file_state) <= 512 ? 1 : -1];

static void put_be32(unsigned char *dst, unsigned long v) {
        dst[0] = (unsigned char)(v >> 24);
        dst[1] = (unsigned char)(v >> 16);
        dst[2] = (unsigned char)(v >> 8);
        dst[3] = (unsigned char)(v >> 0);
}

int init_conn_file(void *conn, const char *file_path) {
        struct file_state *st;
        size_t len;

        len = strlen(file_path);
        if (len == 0 || len >= FILE_PATH_LIMIT) {
                return 1;
        }

        st = (struct file_state *)conn;
        st->offset = 0;
        memcpy(st->path, file_path, len + 1);

        return 0;
}

int send_chunk_file(void *conn, const struct chunk *chunk) {
        struct file_state *st = (struct file_state *)conn;
        FILE *file = fopen(st->path, "a");

        unsigned long size = sizeof(chunk->data);

        fwrite(chunk->data, sizeof(unsigned long), 1, file);

        fwrite(chunk->data, size, 1, file);

        fclose(file);

        return 0;
}

int recv_chunk_file(void *conn, struct chunk *chunk) {
        struct file_state *st;
        FILE *file;
        size_t got;

        st = (struct file_state *)conn;

        file = fopen(st->path, "r");
        fseek(file, (long)st->offset, SEEK_SET);

        got = fread(chunk->data, 1, sizeof chunk->data, file);
        fclose(file);

        put_be32(chunk->length, (unsigned long)got);
        st->offset += (unsigned long)got;

        if (got == 0) {
                chunk->type = 1;
        }

        return 0;
}
