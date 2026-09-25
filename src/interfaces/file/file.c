/*
Placeholder file for now.
The goal is to have multiple "interfaces" which allow the application to transfer data in various ways.
Each interface is supposed to provide the same set of functions for sending a receiving data which is defined in `../interfaces/interfaces.h`.
Using those functions, the user will be able to build a chain of devices to transfer data.
The code in this file along with its counterparts for other ways to transfer data are expected to work on any operating system.
If this cannnot be guaranteed, it must be clearly stated in the README and not compiled for systems with lacking support.

The receive function of the `file` interface reads from a file while the send function writes to a file.
Only the receiving file interface is meant to process chunk types 1 and 2.
*/



#define FILE_APPEND false

int init_conn_file(void *conn, const char *file_path) {
        strncpy(conn, file_path, 512 - 1);

        return 0;
}

int send_chunk_file(void *conn, const struct chunk *chunk) {
        FILE *file = fopen(conn, "a");

        fwrite(&chunk, sizeof(chunk), 1, file);

        return 0;
}

int recv_chunk_file(void *conn, struct chunk *chunk) {
        FILE *file = fopen(conn, "r");

        // TODO: replace 0 with actual size
        // perhaps we add a header to the file that lists chunk size
        // also, this doesn't account for existing chunks within the file
        // perhaps that can be saved in conn
        fread(*chunk, 0, 1, file);
}

