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

Windows/Apple are not implemented yet.

The connection options are:
- "<path>:<baud>" >>> open the serial device at the given baud rate, if it is not provided, BAUD_RATE will be used.
*/

#include <string.h>
#include "serial.h"

#if defined(__linux__)

#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

/* TODO: move to interfaces.h later */
#define CHUNK_TYPE_DATA 0
#define CHUNK_TYPE_PING 3

#define SERIAL_HDR_SIZE 17

/*
Connection state
- fd >>> connected socket
- inited >>> becomes 1 after init_conn_serial succeeded
- ping_outstanding >>> becomes 1 between sending a ping and recieving a reply
*/
struct serial_state {
        int fd;
        unsigned char inited;
        unsigned char ping_outstanding
}

typedef char serial_state_fits_in_slot[sizeof(struct serial_state) <= 512 ? 1 : -1];

/* Map baud rate to termios constant*/
static speed_t lookup_baud(unsigned long v) {
        static const struct {
                unsigned long value;
                speed_t speed;
        } table[] = {
                /* Although this probably isn't ever going to go above 115200, we need to be all inclusive :3*/
                {50UL, B50},
                {75UL, B75},
                {110UL, B110},
                {134UL, B134},
                {150UL, B150},
                {200UL, B200},
                {300UL, B300},
                {600UL, B600},
                {1200UL, B1200},
                {1800UL, B1800},
                {2400UL, B2400},
                {4800UL, B4800},
                {9600UL, B9600},
                {19200UL, B19200},
                {38400UL, B38400},
                {57600UL, B57600},
                {115200UL, B115200},
                {230400UL, B230400},
                {460800UL, B460800},
                {500000UL, B500000},
                {576000UL, B576000},
                {921600UL, B921600},
                {1000000UL, B1000000},
                {1152000UL, B1152000}
        };
        size_t i;

        for (i = 0; i < sizeof table / sizeof table[0]; i++) {
                if (table[i].value == v) {
                        return table[i].speed;
                }
        }

        return 0;
}

/* Split options from <path>[:<baud>] */
static int parse_serial_options(const char *options, char *path, size_t path_size, unsigned long *baud) {
        const char *colon;
        const char *suffix;
        size_t i;
        size_t len;
        unsigned long v;
        int digits;

        if (options == NULL) {
                return SERIAL_ERR_OPTIONS;
        }

        len = strlen(options);
        *baud = (unsigned long)BAUD_RATE;
        colon = strrchr(options, ':');

        if (colon != NULL) {
                suffix = colon + 1;
                if (suffix[0] == '\0') {
                        return SERIAL_ERR_OPTIONS;
                }

                digits = 1;
                for (i = 0; suffix[i] != '\0'; i++) {
                        if (suffix[i] < '0' || suffix[i] > '9') {
                                digits = 0;
                                break;
                        }
                }

                if (digits == 1) {
                        v = 0;
                        for (i = 0; suffix[i] != '\0'; i++) {
                                v = v * 10 + (unsigned long)(suffix[i] - '0');
                                if (v > 1152000UL) {
                                        return SERIAL_ERR_OPTIONS;
                                }
                        }
                        if (lookup_baud(v) == 0) {
                                return SERIAL_ERR_OPTIONS;
                        }
                        
                        *baud = v;
                        len = (size_t)(colon - options);
                }
        } 

        if (len == 0 || len >= path_size) {
                return -1;
        }

        memcpy(path, options, len);
        path[len] = '\0';
        
        return 0;
}

/* Send bytes to peer, retry on EINTR */
static int send_all(int fd, const unsigned char *buf, size_t len) {
        size_t sent;
        ssize_t n;

        sent = 0;
        while (sent < len) {
                n = write(fd, buf + sent, len - sent);
                if (n < 0 && errno == EINTR) {
                        continue;
                }

                if (n <= 0) {
                        return SERIAL_ERR_SEND;
                }
                sent += (size_t)n;
        }

        return SERIAL_OK;
}

/* Read bytes from peer */
static int recv_all(int fd, unsigned char *buf, size_t len) {
        size_t got;
        ssize_t n;

        got = 0;
        while (got < len) {
                n = read(fd, buf + got, len - got);
                if (n < 0 && errno == EINTR) {
                        continue;
                }
                if (n < 0) {
                        if (errno == EIO) {
                                return SERIAL_ERR_CLOSED;
                        }
                        return SERIAL_ERR_RECV;
                }
                if (n == 0) {
                        return SERIAL_ERR_CLOSED;
                }
                got += (size_t)n;
        }

        return SERIAL_OK;
}

/* Most significant byte first order */
static void put_u32(unsigned char *dst, unsigned long v) {
        dst[0] = (unsigned char)(v >> 24);
        dst[1] = (unsigned char)(v >> 16);
        dst[2] = (unsigned char)(v >> 8);
        dst[3] = (unsigned char)(v >> 0);
}

static unsigned long get_u32(const unsigned char *src) {
        return ((unsigned long)src[0] << 24) | ((unsigned long)src[1] << 16) | ((unsigned long)src[2] << 8) | (unsigned long)src[3];
}

/* just read the function name 💔💔💔💔💔 */
static int write_frame(int fd, const struct chunk *c) {
        unsigned char hdr[SERIAL_HDR_SIZE];
        unsigned long len;
        size_t dlen;
        int r;

        len = get_u32(c->length);
        if (len > CHUNK_SIZE) {
                return SERIAL_ERR_ARG;
        }
        dlen = (size_t)len;

        hdr[0] = c->type;
        put_u32(hdr + 1, len);
        put_u32(hdr + 5, get_u32(c->i));
        memcpy(hdr + 9, c->checksum, sizeof c->checksum);

        r = send_all(fd, hdr, sizeof hdr);
        if (r != SERIAL_OK) {
                return r;
        }

        if (dlen > 0) {
                r = send_all(fd, c->data, dlen);
                if (r != SERIAL_OK) {
                        return r;
                }
        }

        return SERIAL_OK;
}

static int read_frame(int fd, struct chunk *c) {
        unsigned char hdr[SERIAL_HDR_SIZE];
        unsigned long len;
        int r;

        r = recv_all(fd, hdr, sizeof hdr);
        if (r != SERIAL_OK) {
                return r;
        }

        len = get_u32(hdr + 1);
        if (len > CHUNK_SIZE) {
                return SERIAL_ERR_FRAME;
        }

        if (len > 0) {
                r = recv_all(fd, c->data, (size_t)len);
                if (r != SERIAL_OK) {
                        return r;
                }
        }

        c->type = hdr[0];
        put_u32(c->length, len);
        put_u32(c->i, get_u32(hdr + 5));
        memcpy(c->checksum, hdr + 9, sizeof c->checksum);

        if (len < CHUNK_SIZE) {
                memset(c->data + (size_t)len, 0, (size_t)(CHUNK_SIZE - len));
        }

        return SERIAL_OK;
}

int init_conn_serial(void *conn, const char *options) {
        struct serial_state *st;
        char path[256];
        unsigned long baud;
        speed_t speed;
        struct termios tty;
        int fd;

        if (conn == NULL) {
                return SERIAL_ERR_ARG;
        }

        st = (struct serial_state *)conn;
        if (st->inited == 1) {
                return SERIAL_ERR_STATE;
        }

        if (parse_serial_options(options, path, sizeof path, &baud) != 0) {
                return SERIAL_ERR_OPTIONS;
        }

        speed = lookup_baud(baud);

        fd = open(path, O_RDWR | O_NOCTTY);
        if (fd < 0) {
                return SERIAL_ERR_OPEN;
        }

        memset(&tty, 0, sizeof tty);
        tty.c_iflag = 0;
        tty.c_oflag = 0;
        tty.c_cflag = CS8 | CREAD | CLOCAL;
        tty.c_lflag = 0;
        tty.c_cc[VMIN] = 1;
        tty.c_cc[VTIME] = 0;

        /* I completely understand what this does. This is not foreshadowing. */
        if (cfsetispeed(&tty, speed) != 0 || cfsetospeed(&tty, speed) != 0 || tcsetattr(fd, TCSANOW, &tty) != 0) {
                close(fd);
                return SERIAL_ERR_CONFIG;
        }

        if (tcflush(fd, TCIOFLUSH) != 0) {
                close(fd);
                return SERIAL_ERR_CONFIG;
        }

        st->fd = fd;
        st-> ping_outstanding = 0;
        st->inited = 1;

        return SERIAL_OK;
}

int send_chunk_serial(void *conn, const struct chunk *chunk) {
        struct serial_state *st;
        int r;

        if (conn == NULL || chunk == NULL) {
                return SERIAL_ERR_ARG;
        }

        st = (struct serial_state *)conn;
        if (st->inited != 1) {
                return SERIAL_ERR_STATE;
        }

        r = write_frame(st->fd, chunk);
        if (r == SERIAL_OK && chunk->type == CHUNK_TYPE_PING) {
                st->ping_outstanding =1;
        }

        return r;
}

int recv_chunk_serial(void *conn, struct chunk *chunk) {
        struct serial_state *st;
        int r;

        if (conn == NULL || chunk == NULL) {
                return SERIAL_ERR_ARG;
        }

        st = (struct serial_state *)conn;
        if (st->inited != 1) {
                return SERIAL_ERR_STATE;
        } 
        
        for (;;) {
                r = read_frame(st->fd, chunk);
                if (r != SERIAL_OK) {
                        return r;
                }

                if (chunk->type != CHUNK_TYPE_PING) {
                        return SERIAL_OK;
                }

                if (st->ping_outstanding != 0) {
                        /* answer to the ping we sent out */
                        st->ping_outstanding = 0;
                        return SERIAL_OK;
                }

                /* write_frame directly so the ping does not set ping_outstanding */
                r = write_frame(st->fd, chunk);
                if (r != SERIAL_OK) {
                        return r;
                }
        }
}

#else
/* windows/apple stubs */

int init_conn_serial(void *conn, const char *port) {
        (void)conn;
        (void)port;
        return SERIAL_ERR_PLATFORM;
}

int send_chunk_serial(void *conn, const struct chunk *chunk) {
        (void)conn;
        (void)chunk;
        return SERIAL_ERR_PLATFORM;
}

int recv_chunk_serial(void *conn, struct chunk *chunk) {
        (void)conn;
        (void)chunk;
        return SERIAL_ERR_PLATFORM;
}

#endif