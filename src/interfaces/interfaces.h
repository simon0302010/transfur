#ifndef INTERFACES_H
#define INTERFACES_H

/* TODO: Allow user to change this */
#define CHUNK_SIZE 4096
/* Context size per interface */
#define INTERFACE_CONTEXT_SIZE 512
/* We should not need more than 8 connections */
#define MAX_CONN 8

/*
Holds a list of all interface types.
Must be passed to `init_conn` in interfaces.c.
*/
enum interface { if_empty, if_lan, if_serial, if_file };

/*
Chunk types
*/
enum chunk_type {
        chunk_type_data,
        chunk_type_finish,
        chunk_type_clear_file,
        chunk_type_ping
};

#if defined(__TURBOC__) || defined(__BORLANDC__)
#pragma option -a-
#elif defined(_MSC_VER)
#pragma pack(push, 1)
#elif defined(__GNUC__) || defined(__clang__)
#define PACKED __attribute__((packed))
#else
#error "Compiler not supported"
#endif

/* `char` types are being used to guarantee 8-bit values. */
struct chunk {
        /*
        All interfaces except `file` interfaces can completely ignore this.

        `enum chunk_type` contains all possible values for this.

        Contains an unsigned 8-bit integer.
        */
        unsigned char type;

        /*
        Length of `data`.
        Contains an unsigned 32-bit integer in big endian order, most
        significant byte first.
        */
        unsigned char length[4];

        /*
        Contains the chunk data with a max size of CHUNK_SIZE in big endian
        order, most significant byte first
        */
        unsigned char data[CHUNK_SIZE];

        /*
        The chunk index.
        Useful for requesting previously failed chunks.
        */
        unsigned char i[4];

        /*
        Checksum which can be used to verify the data.
        Not implementing until basic file transfer is working.
        8 bytes large.
        */
        unsigned char checksum[8];
};

#if defined(__TURBOC__) || defined(__BORLANDC__)
#pragma option -a
#elif defined(_MSC_VER)
#pragma pack(pop)
#endif

/*
Initializes a new connection via the specified protocol.
`void *options` points to a set of options for the interface (ip address, serial
port, etc.). This must be called once per connection and intelligently build a
two-way connection. Specify the type of connection to initialize by passing
`interface`. Pass the pointer of `conn` for the function to write the connection
index into.
*/
int init_conn(int *conn, enum interface interface, void *options);

/*
Sends a chunk of data to the interface.
`recv_chunk` must be running on the receiver end to receive the data.
Returns `0` if the chunk transfer succeeded or any other code based on the
error.
*/
int send_chunk(const int *conn, const struct chunk *chunk);

/*
Waits for a chunk on the receiver side.
Received data is written into `chunk`.
Returns `0` if the chunk transfer succeeded or any other code based on the
error.
*/
int recv_chunk(const int *conn, struct chunk *chunk);

/*
Terminates a connection.
Returns `0` on success, `1` if conn is invalid, or an interface error code
*/
int close_conn(const int *conn);

const char *get_receiver_text(enum interface interface);

const char *get_sender_text(enum interface interface);

/*
Retrives the supported interfaces for the current OS.
Terminated by `if_empty`.
*/
const enum interface *get_supported_interfaces(void);

/*
Returns the description for `code`.
*/
const char *get_error_text(enum interface interface, int code);

#endif