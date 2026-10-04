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

Apple is not implemented yet.

The connection options are:
- "<path>:<baud>" >>> open the serial device at the given baud rate, if it is
not provided, BAUD_RATE will be used. On windows the path is a port name such as
"COM3" or "\\\\.\\COM10"; the
"\\\\.\\" prefix is added automatically when missing.
*/

#include "serial.h"
#include <string.h>

#if defined(OS_LINUX)

#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

typedef int serial_handle;
#define SERIAL_INVALID_HANDLE (-1)

#elif defined(OS_WINDOWS)

#include <windows.h>

typedef HANDLE serial_handle;
#define SERIAL_INVALID_HANDLE INVALID_HANDLE_VALUE

#elif defined(OS_MSDOS)

#include "dos_serial/dos_serial.h"

typedef int serial_handle;
#define SERIAL_INVALID_HANDLE 0

#endif

#if defined(OS_LINUX) || defined(OS_WINDOWS) || defined(OS_MSDOS)

#if defined(OS_LINUX)
static speed_t lookup_baud(unsigned long v);
#elif defined(OS_WINDOWS)
static unsigned long lookup_baud(unsigned long v);
#endif
static int serial_port_open(const char *path, unsigned long baud,
                            serial_handle *out);
static long serial_port_send(serial_handle fd, const unsigned char *buf,
                             size_t len);
static long serial_port_recv(serial_handle fd, unsigned char *buf, size_t len);

#if defined(OS_LINUX)

/* Map baud rate to termios constant*/
static speed_t lookup_baud(unsigned long v) {
        static const struct {
                unsigned long value;
                speed_t speed;
        } table[] = {
            /* Although this probably isn't ever going to go above 115200, we
               need to be all inclusive :3*/
            {50UL, B50},         {75UL, B75},           {110UL, B110},
            {134UL, B134},       {150UL, B150},         {200UL, B200},
            {300UL, B300},       {600UL, B600},         {1200UL, B1200},
            {1800UL, B1800},     {2400UL, B2400},       {4800UL, B4800},
            {9600UL, B9600},     {19200UL, B19200},     {38400UL, B38400},
            {57600UL, B57600},   {115200UL, B115200},   {230400UL, B230400},
            {460800UL, B460800}, {500000UL, B500000},   {576000UL, B576000},
            {921600UL, B921600}, {1000000UL, B1000000}, {1152000UL, B1152000}};
        size_t i;

        for (i = 0; i < sizeof table / sizeof table[0]; i++) {
                if (table[i].value == v) {
                        return table[i].speed;
                }
        }

        return 0;
}

static int serial_port_open(const char *path, unsigned long baud,
                            serial_handle *out) {
        speed_t speed;
        struct termios tty;
        int fd;

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

        speed = lookup_baud(baud);

        /* I completely understand what this does. This is not foreshadowing. */
        if (cfsetispeed(&tty, speed) != 0 || cfsetospeed(&tty, speed) != 0 ||
            tcsetattr(fd, TCSANOW, &tty) != 0) {
                close(fd);
                return SERIAL_ERR_CONFIG;
        }

        if (tcflush(fd, TCIOFLUSH) != 0) {
                close(fd);
                return SERIAL_ERR_CONFIG;
        }

        *out = fd;
        return SERIAL_OK;
}

/* Send bytes to port, retry on EINTR */
static long serial_port_send(serial_handle fd, const unsigned char *buf,
                             size_t len) {
        ssize_t n;

        do {
                n = write(fd, buf, len);
        } while (n < 0 && errno == EINTR);

        return (long)n;
}

/* Read bytes from port, hang up shows up as EIO */
static long serial_port_recv(serial_handle fd, unsigned char *buf, size_t len) {
        ssize_t n;

        do {
                n = read(fd, buf, len);
        } while (n < 0 && errno == EINTR);

        if (n < 0 && errno == EIO) {
                return 0;
        }

        return (long)n;
}

#elif defined(OS_WINDOWS)

/* Validate baud rate, the value itself goes into DCB.BaudRate as-is */
static unsigned long lookup_baud(unsigned long v) {
        static const unsigned long table[] = {
            50UL,     75UL,     110UL,    134UL,    150UL,     200UL,
            300UL,    600UL,    1200UL,   1800UL,   2400UL,    4800UL,
            9600UL,   19200UL,  38400UL,  57600UL,  115200UL,  230400UL,
            460800UL, 500000UL, 576000UL, 921600UL, 1000000UL, 1152000UL};
        size_t i;

        for (i = 0; i < sizeof table / sizeof table[0]; i++) {
                if (table[i] == v) {
                        return v;
                }
        }

        return 0;
}

static int serial_port_open(const char *path, unsigned long baud,
                            serial_handle *out) {
        COMMTIMEOUTS timeouts;
        char full[264];
        size_t plen;
        DCB dcb;
        HANDLE h;

        /* \\.\ lets CreateFileA reach COM10 and above */
        if (strncmp(path, "\\\\.\\", 4) == 0) {
                plen = strlen(path);
                if (plen >= sizeof full) {
                        return SERIAL_ERR_OPTIONS;
                }
                memcpy(full, path, plen + 1);
        } else {
                plen = strlen(path);
                if (plen + 4 >= sizeof full) {
                        return SERIAL_ERR_OPTIONS;
                }
                memcpy(full, "\\\\.\\", 4);
                memcpy(full + 4, path, plen + 1);
        }

        h = CreateFileA(full, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                        OPEN_EXISTING, 0, NULL);
        if (h == INVALID_HANDLE_VALUE) {
                return SERIAL_ERR_OPEN;
        }

        memset(&dcb, 0, sizeof dcb);
        dcb.DCBlength = sizeof dcb;
        if (GetCommState(h, &dcb) == 0) {
                CloseHandle(h);
                return SERIAL_ERR_CONFIG;
        }

        dcb.BaudRate = (DWORD)baud;
        dcb.ByteSize = 8;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT;
        dcb.fBinary = TRUE;
        dcb.fOutxCtsFlow = FALSE;
        dcb.fOutxDsrFlow = FALSE;
        dcb.fOutX = FALSE;
        dcb.fInX = FALSE;
        dcb.fDtrControl = DTR_CONTROL_ENABLE;
        dcb.fRtsControl = RTS_CONTROL_ENABLE;
        dcb.fAbortOnError = TRUE;

        if (SetCommState(h, &dcb) == 0) {
                CloseHandle(h);
                return SERIAL_ERR_CONFIG;
        }

        /* All zero time-outs: block until the full count arrives, like VMIN=1
         */
        memset(&timeouts, 0, sizeof timeouts);
        if (SetCommTimeouts(h, &timeouts) == 0) {
                CloseHandle(h);
                return SERIAL_ERR_CONFIG;
        }

        if (PurgeComm(h, PURGE_TXCLEAR | PURGE_RXCLEAR) == 0) {
                CloseHandle(h);
                return SERIAL_ERR_CONFIG;
        }

        *out = h;
        return SERIAL_OK;
}

static long serial_port_send(serial_handle fd, const unsigned char *buf,
                             size_t len) {
        DWORD written;

        written = 0;
        if (WriteFile(fd, buf, (DWORD)len, &written, NULL) == 0) {
                return -1;
        }

        return (long)written;
}

static long serial_port_recv(serial_handle fd, unsigned char *buf, size_t len) {
        DWORD error;
        DWORD got;
        BOOL ok;

        got = 0;
        ok = ReadFile(fd, buf, (DWORD)len, &got, NULL);
        if (ok == 0) {
                error = GetLastError();
                if (error == ERROR_DEVICE_NOT_CONNECTED ||
                    error == ERROR_BAD_COMMAND ||
                    error == ERROR_INVALID_HANDLE) {
                        /* the adapter went away */
                        return 0;
                }
                return -1;
        }

        return (long)got;
}

#elif defined(OS_MSDOS)

static int serial_port_open(const char *path, unsigned long baud,
                            serial_handle *out) {
        int port;

        /* TODO: Can probably be done in a more efficient way */
        if (strcmp(path, "COM1") == 0) {
                port = COM_1;
        } else if (strcmp(path, "COM2") == 0) {
                port = COM_2;
        } else if (strcmp(path, "COM3") == 0) {
                port = COM_3;
        } else if (strcmp(path, "COM4") == 0) {
                port = COM_4;
        } else {
                return SERIAL_ERR_OPTIONS; /* TODO: I don't know if this is the
                                              correct error code for this */
        }

        if (serial_open(port, (long)baud, 8, 'n', 1, SER_HANDSHAKING_NONE) !=
            SER_SUCCESS)
                return SERIAL_ERR_OPEN;

        return port;
}

static long serial_port_send(serial_handle fd, const unsigned char *buf,
                             size_t len) {
        return serial_write(fd, (const char *)buf, len);
}

static long serial_port_recv(serial_handle fd, unsigned char *buf, size_t len) {
        return serial_read(fd, (char *)buf, len);
}

#endif

#define SERIAL_HDR_SIZE 17

/*
Connection state
- fd >>> connected socket
- inited >>> becomes 1 after init_conn_serial succeeded
- ping_outstanding >>> becomes 1 between sending a ping and recieving a reply
*/
struct serial_state {
        serial_handle fd;
        unsigned char inited;
        unsigned char ping_outstanding;
};

typedef char
    serial_state_fits_in_slot[sizeof(struct serial_state) <= 512 ? 1 : -1];

/* Split options from <path>[:<baud>] */
static int parse_serial_options(const char *options, char *path,
                                size_t path_size, unsigned long *baud) {
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

/* Send bytes to peer */
static int send_all(serial_handle fd, const unsigned char *buf, size_t len) {
        size_t sent;
        long n;

        sent = 0;
        while (sent < len) {
                n = serial_port_send(fd, buf + sent, len - sent);
                if (n <= 0) {
                        return SERIAL_ERR_SEND;
                }
                sent += (size_t)n;
        }

        return SERIAL_OK;
}

/* Read bytes from peer */
static int recv_all(serial_handle fd, unsigned char *buf, size_t len) {
        size_t got;
        long n;

        got = 0;
        while (got < len) {
                n = serial_port_recv(fd, buf + got, len - got);
                if (n < 0) {
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
        return ((unsigned long)src[0] << 24) | ((unsigned long)src[1] << 16) |
               ((unsigned long)src[2] << 8) | (unsigned long)src[3];
}

/* just read the function name 💔💔💔💔💔 */
static int write_frame(serial_handle fd, const struct chunk *c) {
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

static int read_frame(serial_handle fd, struct chunk *c) {
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
        serial_handle fd;
        char path[256];
        unsigned long baud;
        int r;

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

        r = serial_port_open(path, baud, &fd);
        if (r != SERIAL_OK) {
                return r;
        }

        st->fd = fd;
        st->ping_outstanding = 0;
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
        if (r == SERIAL_OK && chunk->type == chunk_type_ping) {
                st->ping_outstanding = 1;
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

                if (chunk->type != chunk_type_ping) {
                        return SERIAL_OK;
                }

                if (st->ping_outstanding != 0) {
                        /* answer to the ping we sent out */
                        st->ping_outstanding = 0;
                        return SERIAL_OK;
                }

                /* write_frame directly so the ping does not set
                 * ping_outstanding */
                r = write_frame(st->fd, chunk);
                if (r != SERIAL_OK) {
                        return r;
                }
        }
}

#else
/* apple stubs 🥀🥀🥀🥀 */

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
