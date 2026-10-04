#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "interfaces.h"

#include "file/file.h"
#include "lan/lan.h"
#include "serial/serial.h"

/* Connection slots */
static unsigned char connections[MAX_CONN][INTERFACE_CONTEXT_SIZE];
static enum interface connection_types[MAX_CONN];

/*
Writes the first empty connection slot index into `int *conn`
Returns `NULL` if no more slots are available.
Otherwise, a pointer to 512 bytes of memory will be returned for the interface
to write its data into.
*/
void *get_new_conn(enum interface if_type, int *conn) {
        int i;

        for (i = 0; i < MAX_CONN; i++) {
                if (connection_types[i] == if_empty) {
                        *conn = i;
                        connection_types[i] = if_type;
                        return connections[i];
                }
        }

        return NULL;
}

/*
Retrives the context of a connection.
Returns NULL if the requested connection doesn't exist.
*/
void *get_conn(const int *conn) {
        if (*conn < 0 || *conn >= MAX_CONN ||
            connection_types[*conn] == if_empty)
                return NULL;

        return connections[*conn];
}

/*
Return values:

0: Success
1: Invalid interface
2: Connection slots full
*/
int init_conn(int *conn, enum interface interface, void *options) {
        void *context;

        if (interface == if_empty) {
                return 1;
        }

        context = get_new_conn(interface, conn);
        if (context == NULL)
                return 2;

        switch (interface) {
        case if_empty:
                break; /* This shouldn't happen */
        case if_file:
                return init_conn_file(context, (const char *)options);
        case if_serial:
                return init_conn_serial(context, (const char *)options);
        case if_lan:
                return init_conn_lan(context, (const char *)options);
        }

        return 1;
}

int send_chunk(const int *conn, const struct chunk *chunk) {
        void *context = get_conn(conn);
        if (context == NULL)
                return 1;

        switch (connection_types[*conn]) {
        case if_empty:
                break;
        case if_file:
                return send_chunk_file(context, chunk);
        case if_serial:
                return send_chunk_serial(context, chunk);
        case if_lan:
                return send_chunk_lan(context, chunk);
        }

        return 1;
}

int recv_chunk(const int *conn, struct chunk *chunk) {
        void *context = get_conn(conn);
        if (context == NULL)
                return 1;

        switch (connection_types[*conn]) {
        case if_empty:
                break;
        case if_file:
                return recv_chunk_file(context, chunk);
        case if_serial:
                return recv_chunk_serial(context, chunk);
        case if_lan:
                return recv_chunk_lan(context, chunk);
        }

        return 1;
}

int close_conn(const int *conn) {
        void *context = get_conn(conn);
        int r = 0;

        if (context == NULL) {
                return 1;
        }

        switch (connection_types[*conn]) {
        case if_empty:
                break; /* How would this even happen lmao */
        case if_file:
                break; /* Nothing needs to happen for file as it doesn't keep any resource open between chunks */
        case if_lan:
                r = close_conn_lan(context);
                break;
        }

        /* This probably creates a bunch of memory leaks lmao */
        memset(context, 0, INTERFACE_CONTEXT_SIZE);
        connection_types[*conn] = if_empty;
        
        return r;
}

const char *get_receiver_text(enum interface interface) {
        switch (interface) {
        case if_empty:
                return "No receiving interface";
        case if_file:
                return "Read from file";
        case if_lan:
                return "Receive over LAN";
        case if_serial:
                return "Receive over serial";
        }
}

const char *get_sender_text(enum interface interface) {
        switch (interface) {
        case if_empty:
                return "No sending interface";
        case if_file:
                return "Write to file";
        case if_lan:
                return "Send over LAN";
        case if_serial:
                return "Send over serial";
        }
}

const enum interface *get_supported_interfaces(void) {
#if defined(OS_LINUX)
        static const enum interface interfaces[] = {if_file, if_lan, if_serial,
                                                    if_empty};
#elif defined(OS_WINDOWS)
        static const enum interface interfaces[] = {if_file, if_lan, if_serial,
                                                    if_empty};
#elif defined(OS_MACOS)
        static const enum interface interfaces[] = {if_file, if_empty};
#elif defined(OS_MSDOS)
        static const enum interface interfaces[] = {if_file, if_serial,
                                                    if_empty};
#else
        static const enum interface interfaces[] = {if_file, if_empty};
        fprintf(stderr,
                "Warning: Operating system not officially supported.\n");
#endif

        return interfaces;
}