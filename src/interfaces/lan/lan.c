/*
The goal is to have multiple "interfaces" which allow the application to
transfer data in various ways. Each interface is supposed to provide the same
set of functions for sending a receiving data which is defined in
`../interfaces/interfaces.h`. Using those functions, the user will be able to
build a chain of devices to transfer data. The code in this file along with its
counterparts for other ways to transfer data are expected to work on any
operating system. If this cannnot be guaranteed, it must be clearly stated in
the README and not compiled for systems with lacking support.

Windows/Apple is not implemented yet.

The connection options are:
- "l:<port>" >>> listen to port on all local addresses and wait for peer to connect
- "l:<ip>:<port>" >>> listen to one specific address
- "c:<ip>:<port>" >>> connect to given peer
*/


#include <string.h>
#include "lan.h"


/* TODO: i'm not sure if apple supports these libs, so check and add accordingly */
#if defined(__linux__)
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef int lan_sock;

#define LAN_INVALID_SOCK (-1)

#elif defined(_WIN32)
/* unimplemented stub */
typedef unsigned long lan_sock;

#define LAN_INVALID_SOCK ((lan_sock)-1)

#endif
static int lan_net_init(void);
static lan_sock lan_socket_open(void);
static int lan_socket_close(lan_sock socket);
static int lan_socket_reuseaddr(lan_sock socket);
static int lan_socket_bind(lan_sock socket, unsigned long ip, unsigned short port);
static int lan_socket_listen(lan_sock socket);
static lan_sock lan_socket_accept(lan_sock socket);
static int lan_socket_connect(lan_sock socket, unsigned long ip, unsigned short port);
static long lan_socket_send(lan_sock socket, const unsigned char *buf, size_t len);
static long lan_socket_recv(lan_sock socket, unsigned char *buf, size_t len);
static void lan_socket_nosigpipe(lan_sock socket);

#if defined(__linux__)

/* Some systems do not have MSG_NOSIGNAL or SO_NOSIGPIPE */
static int lan_net_init(void) {
#if !defined(MSG_NOSIGNAL) && !defined(SO_NOSIGPIPE) && defined(SIGPIPE)
        signal(SIGPIPE, SIG_IGN);
#endif
        return 0;
}

static lan_sock lan_socket_open(void) {
        int fd;

        fd = socket(AF_INET, SOCK_STREAM, 0);

        if (fd < 0) {
                return LAN_INVALID_SOCK;
        }

        return fd;
}

static int lan_socket_close(lan_sock socket) {
        return close(socket) == 0 ? 0 : -1;
}

/* Reuse socket instantly without waiting TIME_WAIT*/
static int lan_socket_reuseaddr(lan_sock socket) {
        int on = 1;

        return setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on) == 0 ? 0 : -1;
}

/* Bind a listening socket, ip = 0 binds to all addresses */
static int lan_socket_bind(lan_sock socket, unsigned long ip, unsigned short port) {
        struct sockaddr_in socket_address;

        memset(&socket_address, 0, sizeof socket_address);
        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htons(port);
        socket_address.sin_addr.s_addr = htonl(ip);

        return bind(socket, (struct sockaddr *)&socket_address, sizeof socket_address) == 0 ? 0 : -1;
}

/* Mark socket as listening */
static int lan_socket_listen(lan_sock socket) {
        return listen(socket, MAX_CONN) == 0 ? 0 : -1;
}

/* Wait for peer to connect and return connected socket, this is what makes a
   "l:" connection block until the other device shows up */
static lan_sock lan_socket_accept(lan_sock socket) {
        int fd;

        fd = accept(socket, NULL, NULL);

        if (fd < 0) {
                return LAN_INVALID_SOCK;
        }

        return fd;
}

/* Connect to peer, ip and port are in host byte order */
static int lan_socket_connect(lan_sock socket, unsigned long ip, unsigned short port) {
        struct sockaddr_in socket_address;

        memset(&socket_address, 0, sizeof socket_address);

        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htons(port);
        socket_address.sin_addr.s_addr = htonl(ip);

        return connect(socket, (struct sockaddr *)&socket_address, sizeof socket_address) == 0 ? 0 : -1;
}

/* Send bytes to peer */
static long lan_socket_send(lan_sock socket, const unsigned char *buf, size_t len) {
        ssize_t n;

        do {
#ifdef MSG_NOSIGNAL
                n = send(socket, buf, len, MSG_NOSIGNAL);
#else
                n = send(socket, buf, len, 0);
#endif
        } while (n < 0 && errno == EINTR);

        return (long)n;
}

/* Receive bytes from peer */
static long lan_socket_recv(lan_sock socket, unsigned char *buf, size_t len) {
        ssize_t n;

        do {
                n = recv(socket, buf, len, 0);
        } while (n < 0 && errno == EINTR);

        return (long)n;
}

/* Don't raise SIGPIPE for systems without MSG_NOSIGNAL */
static void lan_socket_nosigpipe(lan_sock socket) {
#ifdef SO_NOSIGPIPE
        int on = 1;

        setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof on);
#else
        (void)socket;
#endif
}

#elif defined(_WIN32)

/* Windows Stubs */
static int lan_net_init(void) {
        return LAN_ERR_PLATFORM;
}

static lan_sock lan_socket_open(void) {
        return LAN_ERR_PLATFORM;
}

static int lan_socket_close(lan_sock socket) {
        return LAN_ERR_PLATFORM;
}

static int lan_socket_reuseaddr(lan_sock socket) {
        return LAN_ERR_PLATFORM;
}

static int lan_socket_bind(lan_sock socket, unsigned long ip, unsigned short port) {
        return LAN_ERR_PLATFORM;

}

static int lan_socket_listen(lan_sock socket) {
        return LAN_ERR_PLATFORM;
}

static lan_sock lan_socket_accept(lan_sock socket) {
        return LAN_ERR_PLATFORM;
}

static int lan_socket_connect(lan_sock socket, unsigned long ip, unsigned short port) {
        return LAN_ERR_PLATFORM;
}

static long lan_socket_send(lan_sock socket, const unsigned char *buf, size_t len) {
        return LAN_ERR_PLATFORM;
}

static long lan_socket_recv(lan_sock socket, unsigned char *buf, size_t len) {
        return LAN_ERR_PLATFORM;
}

static void lan_socket_nosigpipe(lan_sock socket) {
        return LAN_ERR_PLATFORM;

}

#endif

/* TODO: move to interfaces.h later*/
#define CHUNK_TYPE_DATA 0
#define CHUNK_TYPE_PING 3

#define LAN_ROLE_LISTEN 1
#define LAN_ROLE_CONNECT 2

#define LAN_HDR_SIZE 17

static const unsigned char lan_zeroes[CHUNK_SIZE] = {0};


/*
Connection state
- fd >>> connected socket
- port >>> communications port
- role >>> whether we're listening or connnecting, LAN_ROLE_CONNECT or LAN_ROLE_LISTEN
- inited >>> becomes 1 after init_conn_lan succeeded
- ping_outstanding >>> becomes 1 between sending a ping and recieving a reply
*/
struct lan_state {
        lan_sock fd;
        unsigned short port;
        unsigned char role;
        unsigned char inited;
        unsigned char ping_outstanding;
};

typedef char lan_state_fits_in_slot[sizeof(struct lan_state) <= 512 ? 1 : -1];

/* Send bytes to peer */
static int send_all(lan_sock socket, const unsigned char *buf, size_t len) {
        size_t sent;
        long n;

        sent = 0;
        while (sent < len) {
                n = lan_socket_send(socket, buf + sent, len - sent);
                if (n <= 0) {
                        return LAN_ERR_SEND;
                }
                sent += (size_t)n;
        }

        return LAN_OK;
}

/* Read bytes from peer, return LAN_OK, LAN_ERR_RECV which is self-explanatory and LAN_ERR_CLOSED on peer hang up */
static int recv_all(lan_sock socket, unsigned char *buf, size_t len) {
        size_t got;
        long n;

        got = 0;
        while (got < len) {
                n = lan_socket_recv(socket, buf + got, len - got);
                if (n < 0) {
                        return LAN_ERR_RECV;
                }
                if (n == 0) {
                        return LAN_ERR_CLOSED;
                }
                got += (size_t)n;
        }

        return LAN_OK;
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

static int write_frame(lan_sock socket, const struct chunk *c) {
        unsigned char hdr[LAN_HDR_SIZE];
        unsigned long len;
        size_t dlen;
        int r;

        len = get_u32(c->length);
        if (len > CHUNK_SIZE) {
                return LAN_ERR_ARG;
        }
        dlen = (size_t)len;

        hdr[0] = c->type;
        put_u32(hdr + 1, len);
        put_u32(hdr + 5, get_u32(c->i));
        memcpy(hdr + 9, c->checksum, sizeof c->checksum);

        r = send_all(socket, hdr, sizeof hdr);
        if (r != LAN_OK) {
                return r;
        }

        if (c->type != CHUNK_TYPE_DATA) {
                return send_all(socket, lan_zeroes, CHUNK_SIZE);
        }

        if (dlen > 0) {
                r = send_all(socket, c->data, dlen);
                if (r != LAN_OK) {
                        return r;
                }
        }

        return send_all(socket, lan_zeroes, CHUNK_SIZE - dlen);
}

static int read_frame(lan_sock socket, struct chunk *c) {
        unsigned char hdr[LAN_HDR_SIZE];
        unsigned long len;
        int r;

        r = recv_all(socket, hdr, sizeof hdr);
        if (r != LAN_OK) {
                return r;
        }

        len = get_u32(hdr + 1);
        if (len > CHUNK_SIZE) {
                return LAN_ERR_FRAME;
        }

        r = recv_all(socket, c->data, CHUNK_SIZE);
        if (r != LAN_OK) {
                return r;
        }

        c->type = hdr[0];
        put_u32(c->length, len);
        put_u32(c->i, get_u32(hdr + 5));
        memcpy(c->checksum, hdr + 9, sizeof c->checksum);

        if (len < CHUNK_SIZE) {
                memset(c->data + (size_t)len, 0, (size_t)(CHUNK_SIZE - len));
        }

        return LAN_OK;
}

static int parse_port(const char *s, size_t len, unsigned short *out) {
        unsigned long v;
        size_t i;

        if (len == 0) {
                return -1;
        }

        v = 0;
        for (i = 0; i < len; i++) {
                if (s[i] < '0' || s[i] > '9') {
                        return -1;
                }
                v = v * 10 + (unsigned long)(s[i] - '0');
                if (v > 65535UL) {
                        return -1;
                }
        }

        if (v == 0) {
                return -1;
        }

        *out = (unsigned short)v;
        return 0;
}

static int parse_ip(const char *s, size_t len, unsigned long *out) {
        unsigned long v;
        unsigned long octet;
        size_t i;
        size_t start;
        int parts;

        if (len == 0) {
                return -1;
        }

        v = 0;
        parts = 0;
        i = 0;

        while (i < len) {
                octet = 0;
                start = i;
                while (i < len && s[i] != '.') {
                        if (s[i] < '0' || s[i] > '9') {
                                return -1;
                        }
                        octet = octet * 10 + (unsigned long)(s[i] - '0');
                        if (octet > 255UL) {
                                return -1;
                        }
                        i++;
                }

                if (i == start) { /* empty octet, as in "1..2.3" */
                        return -1;
                }

                parts++;
                if (parts == 4) {
                        break;
                }

                v = (v << 8) | octet;

                if (i >= len || s[i] != '.') {
                        return -1;
                }
                i++;
        }

        if (parts != 4 || i != len) {
                return -1;
        }

        *out = (v << 8) | octet;
        return 0;
}

/* Parse role, ip and port from <role>:<ip>:<port> */
static int parse_options(const char *options, unsigned char *role,
                         unsigned long *addr, unsigned short *port) {
        const char *rest;
        const char *colon;

        if (options == NULL) {
                return -1;
        }

        if (options[0] == 'l' && options[1] == ':') {
                *role = LAN_ROLE_LISTEN;
                rest = options + 2;
        } else if (options[0] == 'c' && options[1] == ':') {
                *role = LAN_ROLE_CONNECT;
                rest = options + 2;
        } else {
                return -1;
        }

        colon = strrchr(rest, ':');
        if (colon == NULL) {
                if (*role != LAN_ROLE_LISTEN) {
                        return -1;
                }
                *addr = 0;
                return parse_port(rest, strlen(rest), port);
        }

        if (parse_ip(rest, (size_t)(colon - rest), addr) != 0) {
                return -1;
        }

        return parse_port(colon + 1, strlen(colon + 1), port);
}


int init_conn_lan(void *conn, const char *options) {
        struct lan_state *st;
        unsigned char role;
        unsigned long addr;
        unsigned short port;
        lan_sock fd;
        lan_sock listener;
        int r;

        if (conn == NULL) {
                return LAN_ERR_ARG;
        }

        st = (struct lan_state *)conn;
        if (st->inited == 1) {
                return LAN_ERR_STATE;
        }

        if (parse_options(options, &role, &addr, &port) != 0) {
                return LAN_ERR_OPTIONS;
        }

        r = lan_net_init();
        if (r != 0) {
                return r;
        }

        fd = lan_socket_open();
        if (fd == LAN_INVALID_SOCK) {
                return LAN_ERR_SOCKET;
        }

        lan_socket_nosigpipe(fd);

        if (role == LAN_ROLE_LISTEN) {
                lan_socket_reuseaddr(fd);

                if (lan_socket_bind(fd, addr, port) != 0) {
                        lan_socket_close(fd);
                        return LAN_ERR_BIND;
                }

                if (lan_socket_listen(fd) != 0) {
                        lan_socket_close(fd);
                        return LAN_ERR_LISTEN;
                }

                listener = fd;
                fd = lan_socket_accept(listener);
                lan_socket_close(listener);
                if (fd == LAN_INVALID_SOCK) {
                        return LAN_ERR_ACCEPT;
                }

                lan_socket_nosigpipe(fd);
        } else if (lan_socket_connect(fd, addr, port) != 0) {
                lan_socket_close(fd);
                return LAN_ERR_CONNECT;
        }

        st->fd = fd;
        st->port = port;
        st->role = role;
        st->ping_outstanding = 0;
        st->inited = 1;

        return LAN_OK;
}

int send_chunk_lan(void *conn, const struct chunk *chunk) {
        struct lan_state *st;
        int r;

        if (conn == NULL || chunk == NULL) {
                return LAN_ERR_ARG;
        }

        st = (struct lan_state *)conn;
        if (st->inited != 1) {
                return LAN_ERR_STATE;
        }

        r = write_frame(st->fd, chunk);
        if (r == LAN_OK && chunk->type == CHUNK_TYPE_PING) {
                st->ping_outstanding = 1;
        }

        return r;
}

int recv_chunk_lan(void *conn, struct chunk *chunk) {
        struct lan_state *st;
        int r;

        if (conn == NULL || chunk == NULL) {
                return LAN_ERR_ARG;
        }

        st = (struct lan_state *)conn;
        if (st->inited != 1) {
                return LAN_ERR_STATE;
        }

        for (;;) {
                r = read_frame(st->fd, chunk);
                if (r != LAN_OK) {
                        return r;
                }

                if (chunk->type != CHUNK_TYPE_PING) {
                        return LAN_OK;
                }

                if (st->ping_outstanding != 0) {
                        /* The answer to the ping we sent out */
                        st->ping_outstanding = 0;
                        return LAN_OK;
                }

                /* write_frame directly so the ping does not set ping_outstanding and turn into our own ping */
                r = write_frame(st->fd, chunk);
                if (r != LAN_OK) {
                        return r;
                }
        }
}

#endif
