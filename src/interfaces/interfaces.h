#ifndef INTERFACES_H
#define INTERFACES_H

/* TODO: Allow user to change this */
#define CHUNK_SIZE 4096
/* We should not need more than 8 connections */
#define MAX_CONN 8

/*
Holds a list of all interface types.
Must be passed to `init_conn` in interfaces.c.
*/
enum interface { if_empty, if_lan, if_serial, if_file };

/* `char` types are being used to guarantee 8-bit values. */
struct chunk {
        /*
        Transmission chunk types:

        0: A regular chunk containing binary data

        1: Transfer finish message. `data` is expected to stay empty. A `file`
        interface is supposed to consume this while other interfaces pass it on.

        2: Request to clear buffer on the last `file` interface in the chain. A
        `file` interface is supposed to consume this while other interfaces pass
        it on.

        3: Ping message. Any receive interface is supposed to sent the entire
        chunk back. The first receiver consumes this and sends a chunk back.
        Sender must be waiting for chunks when sending this to get the response.

        Contains an unsigned 8-bit integer.
        */
        unsigned char type;

        /*
        Length of `data`.
        Contains an unsigned 32-bit integer
        */
        unsigned char length[4];

        /*
        Contains the chunk data with a max size of CHUNK_SIZE.
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
Clears the receive buffer on the ending `file` interface.
The last device in a chain is supposed to have a `file` interface for the last
sender to save the received data into a file. Call this before starting a new
transfer. Automatically called after `finish_transfer` is called.
*/
int clear_recv_buffer(const int *conn);

/*
Sends the finish transfer message along the chain.
The ending `file` interface is supposed to use this signal to save its buffer
into the configured file. Called when the first sender is looking to finish the
transfer.
*/
int finish_transfer(const int *conn);

#endif