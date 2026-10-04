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

#include "../../misc/debug.h"
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

static unsigned long get_be32(const unsigned char *src) {
        return ((unsigned long)src[0] << 24) | ((unsigned long)src[1] << 16) |
               ((unsigned long)src[2] << 8) | (unsigned long)src[3];
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
        strcpy(st->path, file_path);

        return 0;
}

int send_chunk_file(void *conn, const struct chunk *chunk) {
        struct file_state *st;
        FILE *file;
        unsigned long len;
        size_t wrote;

        st = (struct file_state *)conn;

        len = get_be32(chunk->length);
        if (len > (unsigned long)CHUNK_SIZE) {
                return 1;
        }

        print_debug(
            "file interface %p received a chunk of type %i and length %lu\n",
            conn, chunk->type, len);

        if (chunk->type == chunk_type_clear_file) {
                /* Empties the file */
                FILE *delfile = fopen(st->path, "w");
                if (delfile) {
                        fclose(delfile);
                        print_debug("deleting contents of %s\n", st->path);
                        return 0;
                } else {
                        fprintf(stderr,
                                "file interface %p failed to delete contents "
                                "of %s\n",
                                conn, st->path);
                        return 1;
                }
        } else if (chunk->type == chunk_type_finish) {
                if (len > 0)
                        print_debug("warning: file interface %p received final "
                                    "chunk with length %lu\n",
                                    conn, len);
                return 0;
        } else if (chunk->type != chunk_type_data) {
                print_debug(
                    "file interface %p received incompatible chunk type: %i\n",
                    conn, chunk->type);
                return 1;
        }

        file = fopen(st->path, "ab");
        if (file == NULL) {
                return 1;
        }

        wrote = fwrite(chunk->data, 1, (size_t)len, file);
        if (fclose(file) != 0 || wrote != (size_t)len) {
                return 1;
        }

        return 0;
}

int recv_chunk_file(void *conn, struct chunk *chunk) {
        struct file_state *st;
        FILE *file;
        size_t got;

        st = (struct file_state *)conn;

        file = fopen(st->path, "rb");
        if (file == NULL) {
                return 1;
        }
        fseek(file, (long)st->offset, SEEK_SET);

        got = fread(chunk->data, 1, sizeof chunk->data, file);
        fclose(file);

        put_be32(chunk->length, (unsigned long)got);
        st->offset += (unsigned long)got;

        if (got == 0) {
                chunk->type = chunk_type_finish;
        } else {
                chunk->type = chunk_type_data;
        }

        return 0;
}
