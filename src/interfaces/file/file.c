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

struct FileConnection {
        unsigned long position;
        char file_path[512 - sizeof(unsigned long)];
};

int init_conn_file(struct FileConnection *conn, const char *file_path) {

        conn->position = 0; /* TODO: Update with any existing chunks ignored */

        strncpy(conn->file_path, file_path, sizeof(conn->file_path));

        return 0;
}

int send_chunk_file(struct FileConnection *conn, const struct chunk *chunk) {
        FILE *file = fopen(conn->file_path, "a");

        unsigned long size = sizeof(chunk->data);

        fwrite(chunk->data, sizeof(unsigned long), 1, file);

        fwrite(chunk->data, size, 1, file);

        fclose(file);

        return 0;
}

int recv_chunk_file(struct FileConnection *conn, struct chunk *chunk) {
        FILE *file = fopen(conn->file_path, "r");

        int position = 0;
        unsigned long size;

        while (position < conn->position) {
                fread(&size, sizeof(unsigned long), 1, file);

                fseek(file, size, SEEK_CUR);
        }

        fread(&size, sizeof(unsigned long), 1, file);

        fread(chunk->data, size, 1, file);

        fclose(file);

        return 0;
}
