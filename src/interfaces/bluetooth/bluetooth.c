/*
Transfer data over Bluetooth RFCOMM

The connection options are:
- "l:<channel>" >>> listen on <channel> on all local adapters and wait for a
peer to connect
- "l:<bdaddr>:<channel>" >>> listen to one specific adapter
- "c:<bdaddr>:<channel>" >>> connect to peer

Apple is not implemented
*/

#include "bluetooth.h"

#if (defined(OS_LINUX) && defined(HAS_BLUETOOTH)) ||                           \
    defined(OS_LINUX) /* Supported systems */

#include <string.h>

#if defined(OS_LINUX)

#define inline __inline__

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#undef inline

#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef int bt_sock;

#define BT_INVALID_SOCK (-1)

#elif defined(OS_WINDOWS)

#include <winsock2.h>
#include <ws2bth.h>

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef SOCKET bt_sock;

#define BT_INVALID_SOCK INVALID_SOCKET

#endif

#define BT_HDR_SIZE 17

static const unsigned char bt_zeroes[CHUNK_SIZE] = {0};

static int bt_net_init(void);
static bt_sock bt_socket_open(void);
static int bt_socket_close(bt_sock socket);

#if defined(OS_LINUX)
static int bt_socket_bind(bt_sock socket, const bdaddr_t *addr,
                          unsigned int channel);
#elif defined(OS_WINDOWS)
static int bt_socket_bijnd(bt_sock socket, const BTH_ADDR *addr,
                           unsigned int channel);
#endif
static int bt_socket_listen(bt_sock socket);
static bt_sock bt_socket_accept(bt_sock socket);
#if defined(OS_LINUX)
static int bt_socket_connect(bt_sock socket, const bdaddr_t *addr,
                             unsigned int channel);
#elif defined(OS_WINDOWS)
static int bt_socket_connect(bt_sock socket, const BTH_ADDR *addr,
                             unsigned int channel);
static int bt_addr_from_str(const char *str, BTH_ADDR *out);
#endif
static long bt_socket_send(bt_sock socket, const unsigned char *buf,
                           size_t len);
static long bt_socket_recv(bt_sock socket, unsigned char *buf, size_t len);
static void bt_socket_nosigpipe(bt_sock socket);

static int bt_net_init(void) {
#if defined(OS_LINUX)
#if !defined(MSG_NOSIGNAL) && !defined(SO_NOSIGPIPE) && defined(SIGPIPE)
        signal(SIGPIPE, SIG_IGN);
#endif
        return 0;
#elif defined(OS_WINDOWS)
        WSADATA wsa;
        static int inited = 0;

        if (inited) {
                return 0;
        }

        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
                return -1;
        }
        inited = 1;
        return 0;
#else
        return 0;
#endif
}

static bt_sock bt_socket_open(void) {
        bt_sock fd;
#if defined(OS_LINUX)
        fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
#elif defined(OS_WINDOWS)
        fd = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
#else
        return BT_INVALID_SOCK;
#endif
        if (fd < 0) {
                return BT_INVALID_SOCK;
        }

        return fd;
}

static int bt_socket_close(bt_sock socket) {
#if defined(OS_LINUX)
        return close(socket) == 0 ? 0 : -1;
#elif defined(OS_WINDOWS)
        if (socket == BT_INVALID_SOCK) {
                return 0;
        }
        return closesocket(socket) == 0 ? 0 : -1;
#else
        (void)socket;
        return -1;
#endif
}

#if defined(OS_LINUX)
/* Bind a listening socket, addr = BDADDR_ANY binds to all adapters */
static int bt_socket_bind(bt_sock socket, const bdaddr_t *addr,
                          unsigned int channel) {
        struct sockaddr_rc socket_address;

        memset(&socket_address, 0, sizeof socket_address);
        socket_address.rc_family = AF_BLUETOOTH;
        socket_address.rc_bdaddr = *addr;
        socket_address.rc_channel = (unsigned char)channel;

        return bind(socket, (struct sockaddr *)&socket_address,
                    sizeof socket_address) == 0
                   ? 0
                   : -1;
}
#elif defined(OS_WINDOWS)
static int bt_socket_bind(bt_sock socket, const BTH_ADDR *addr,
                          unsigned int channel) {
        SOCKADDR_BTH socket_address;

        memset(&socket_address, 0, sizeof socket_address);
        socket_address.addressFamily = AF_BTH;
        if (addr == NULL) {
                socket_address.btAddr = 0;
        } else {
                socket_address.btAddr = *addr;
        }
        socket_address.port = (ULONG)channel;
        socket_address.serviceClassId = RFCOMM_PROTOCOL_UUID;

        return bind(socket, (struct sockaddr *)&socket_address,
                    sizeof socket_address) == 0
                   ? 0
                   : -1;
}
#endif

static int bt_socket_listen(bt_sock socket) {
        return listen(socket, MAX_CONN) == 0 ? 0 : -1;
}

static bt_sock bt_socket_accept(bt_sock socket) {
#if defined(OS_LINUX)
        int fd;

        fd = accept(socket, NULL, NULL);

        if (fd < 0) {
                return BT_INVALID_SOCK;
        }

        return fd;
#elif defined(OS_WINDOWS)
        SOCKET fd;

        fd = accept(socket, NULL, NULL);

        if (fd == INVALID_SOCKET) {
                return BT_INVALID_SOCK;
        }

        return fd;
#else
        (void)socket;
        return BT_INVALID_SOCK;
#endif
}

/* Connect to Mister Peer */
#if defined(OS_LINUX)
static int bt_socket_connect(bt_sock socket, const bdaddr_t *addr,
                             unsigned int channel) {
        struct sockaddr_rc socket_address;

        memset(&socket_address, 0, sizeof socket_address);
        socket_address.rc_family = AF_BLUETOOTH;
        socket_address.rc_bdaddr = *addr;
        socket_address.rc_channel = (unsigned char)channel;

        return connect(socket, (struct sockaddr *)&socket_address,
                       sizeof socket_address) == 0
                   ? 0
                   : -1;
}
#elif defined(OS_WINDOWS)
static int bt_socket_connect(bt_sock socket, const BTH_ADDR *addr,
                             unsigned int channel) {
        SOCKADDR_BTH socket_address;

        if (addr == NULL) {
                return -1;
        }

        memset(&socket_address, 0, sizeof socket_address);
        socket_address.addressFamily = AF_BTH;
        socket_address.btAddr = *addr;
        socket_address.port = (ULONG)channel;
        socket_address.serviceClassId = RFCOMM_PROTOCOL_UUID;

        return connect(socket, (struct sockaddr *)&socket_address,
                       sizeof socket_address) == 0
                   ? 0
                   : -1;
}

static int bt_addr_from_str(const char *str, BTH_ADDR *out) {
        BTH_ADDR addr = 0;
        int i;
        const char *p = str;

        if (str == NULL || out == NULL) {
                return -1;
        }

        for (i = 0; i < 6; i++) {
                unsigned int b = 0;
                char c1 = p[0];
                char c2 = p[1];

                if (c1 == '\0' || c2 == '\0') {
                        return -1;
                }

                if (c1 >= '0' && c1 <= '9') {
                        b |= ((c1 - '0') << 4);
                } else if (c1 >= 'a' && c1 <= 'f') {
                        b |= ((c1 - 'a' + 10) << 4);
                } else if (c1 >= 'A' && c1 <= 'F') {
                        b |= ((c1 - 'A' + 10) << 4);
                } else {
                        return -1;
                }

                if (c2 >= '0' && c2 <= '9') {
                        b |= (c2 - '0');
                } else if (c2 >= 'a' && c2 <= 'f') {
                        b |= (c2 - 'a' + 10);
                } else if (c2 >= 'A' && c2 <= 'F') {
                        b |= (c2 - 'A' + 10);
                } else {
                        return -1;
                }

                addr = (addr << 8) | (BTH_ADDR)(b & 0xFF);
                p += 2;
                if (i < 5) {
                        if (*p != ':') {
                                return -1;
                        }
                        p++;
                }
        }

        if (*p != '\0') {
                return -1;
        }

        *out = addr;
        return 0;
}
#endif

/* Send bytes to peer */
static long bt_socket_send(bt_sock socket, const unsigned char *buf,
                           size_t len) {
#if defined(OS_LINUX)
        ssize_t n;

        do {
#ifdef MSG_NOSIGNAL
                n = send(socket, buf, len, MSG_NOSIGNAL);
#else
                n = send(socket, buf, len, 0);
#endif
        } while (n < 0 && errno == EINTR);

        return (long)n;
#elif defined(OS_WINDOWS)
        int n;

        do {
                n = send(socket, (const char *)buf, (int)len, 0);
        } while (n == SOCKET_ERROR && WSAGetLastError() == WSAEINTR);

        if (n == SOCKET_ERROR) {
                return -1;
        }

        return (long)n;
#else
        (void)socket;
        (void)buf;
        (void)len;
        return -1;
#endif
}

/* Receive bytes from peer */
static long bt_socket_recv(bt_sock socket, unsigned char *buf, size_t len) {
#if defined(OS_LINUX)
        ssize_t n;

        do {
                n = recv(socket, buf, len, 0);
        } while (n < 0 && errno == EINTR);

        return (long)n;
#elif defined(OS_WINDOWS)
        int n;

        do {
                n = recv(socket, (char *)buf, (int)len, 0);
        } while (n == SOCKET_ERROR && WSAGetLastError() == WSAEINTR);

        if (n == SOCKET_ERROR) {
                return -1;
        }

        return (long)n;
#else
        (void)socket;
        (void)buf;
        (void)len;
        return -1;
#endif
}

static void bt_socket_nosigpipe(bt_sock socket) {
#if defined(OS_LINUX)
#ifdef SO_NOSIGPIPE
        int on = 1;

        setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof on);
#else
        (void)socket;
#endif
#elif defined(OS_WINDOWS)
        (void)socket;
#else
        (void)socket;
#endif
}

#define BT_ROLE_LISTEN 1
#define BT_ROLE_CONNECT 2

#define BT_CHANNEL_MIN 1
#define BT_CHANNEL_MAX 30

/*
Connection state
- fd >>> connected socket
- role >>> BT_ROLE_LISTEN or BT_ROLE_CONNECT
- inited >>> becomes 1 after init_conn_bluetooth succeded
- ping_outstanding >>> becomes 1 between sending a ping and receiving a reply
*/
struct bt_state {
        bt_sock fd;
        unsigned char role;
        unsigned char inited;
        unsigned char ping_outstanding;
};

typedef char bt_state_fits_in_slot[sizeof(struct bt_state) <= 512 ? 1 : -1];

/* Send bytes to peer */
static int send_all(bt_sock socket, const unsigned char *buf, size_t len) {
        size_t sent;
        long n;

        sent = 0;
        while (sent < len) {
                n = bt_socket_send(socket, buf + sent, len - sent);
                if (n <= 0) {
                        return BT_ERR_SEND;
                }
                sent += (size_t)n;
        }

        return BT_OK;
}

/* Read bytes from peer, BT_ERR_CLOSED on peer hang up */
static int recv_all(bt_sock socket, unsigned char *buf, size_t len) {
        size_t got;
        long n;

        got = 0;
        while (got < len) {
                n = bt_socket_recv(socket, buf + got, len - got);
                if (n < 0) {
                        return BT_ERR_RECV;
                }
                if (n == 0) {
                        return BT_ERR_CLOSED;
                }
                got += (size_t)n;
        }

        return BT_OK;
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

static int write_frame(bt_sock socket, const struct chunk *c) {
        unsigned char hdr[BT_HDR_SIZE];
        unsigned long len;
        size_t dlen;
        int r;

        len = get_u32(c->length);
        if (len > CHUNK_SIZE) {
                return BT_ERR_ARG;
        }
        dlen = (size_t)len;

        hdr[0] = c->type;
        put_u32(hdr + 1, len);
        put_u32(hdr + 5, get_u32(c->i));
        memcpy(hdr + 9, c->checksum, sizeof c->checksum);

        r = send_all(socket, hdr, sizeof hdr);
        if (r != BT_OK) {
                return r;
        }

        if (c->type != chunk_type_data) {
                return send_all(socket, bt_zeroes, CHUNK_SIZE);
        }

        if (dlen > 0) {
                r = send_all(socket, c->data, dlen);
                if (r != BT_OK) {
                        return r;
                }
        }

        return send_all(socket, bt_zeroes, CHUNK_SIZE - dlen);
}

static int read_frame(bt_sock socket, struct chunk *c) {
        unsigned char hdr[BT_HDR_SIZE];
        unsigned long len;
        int r;

        r = recv_all(socket, hdr, sizeof hdr);
        if (r != BT_OK) {
                return r;
        }

        len = get_u32(hdr + 1);
        if (len > CHUNK_SIZE) {
                return BT_ERR_FRAME;
        }

        r = recv_all(socket, c->data, CHUNK_SIZE);
        if (r != BT_OK) {
                return r;
        }

        c->type = hdr[0];
        put_u32(c->length, len);
        put_u32(c->i, get_u32(hdr + 5));
        memcpy(c->checksum, hdr + 9, sizeof c->checksum);

        if (len < CHUNK_SIZE) {
                memset(c->data + (size_t)len, 0, (size_t)(CHUNK_SIZE - len));
        }

        return BT_OK;
}

static int parse_channel(const char *s, size_t len, unsigned int *out) {
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
                if (v > (unsigned long)BT_CHANNEL_MAX) {
                        return -1;
                }
        }

        if (v < (unsigned long)BT_CHANNEL_MIN) {
                return -1;
        }

        *out = (unsigned int)v;

        return 0;
}

static int parse_options(const char *options, unsigned char *role,
                         char addr_text[18], int *have_addr,
                         unsigned int *channel) {
        const char *rest;
        const char *colon;
        size_t len;

        if (options == NULL) {
                return -1;
        }

        addr_text[0] = '\0';
        *have_addr = 0;

        if (options[0] == 'l' && options[1] == ':') {
                *role = BT_ROLE_LISTEN;
                rest = options + 2;
        } else if (options[0] == 'c' && options[1] == ':') {
                *role = BT_ROLE_CONNECT;
                rest = options + 2;
        } else {
                return -1;
        }

        colon = strrchr(rest, ':');
        if (colon == NULL) {
                if (*role != BT_ROLE_LISTEN) {
                        return -1;
                }
                return parse_channel(rest, strlen(rest), channel);
        }

        len = (size_t)(colon - rest);
        if (len == 0 || len >= 18) {
                return -1;
        }
        memcpy(addr_text, rest, len);
        addr_text[len] = '\0';

        *have_addr = 1;

        return parse_channel(colon + 1, strlen(colon + 1), channel);
}

int init_conn_bluetooth(void *conn, const char *options) {
        struct bt_state *st;
        unsigned char role;
        char addr_text[18];
        int have_addr;
        unsigned int channel;
        bt_sock fd;
        bt_sock listener;
        int r;
#if defined(OS_LINUX)
        bdaddr_t addr;
        bdaddr_t any = {0};
#endif

        if (conn == NULL) {
                return BT_ERR_ARG;
        }

        st = (struct bt_state *)conn;
        if (st->inited == 1) {
                return BT_ERR_STATE;
        }

        if (parse_options(options, &role, addr_text, &have_addr, &channel) !=
            0) {
                return BT_ERR_OPTIONS;
        }

        r = bt_net_init();
        if (r != 0) {
                return r;
        }

        fd = bt_socket_open();
        if (fd == BT_INVALID_SOCK) {
                return BT_ERR_SOCKET;
        }

        bt_socket_nosigpipe(fd);

#if defined(OS_LINUX)
        if (have_addr) {
                if (str2ba(addr_text, &addr) != 0) {
                        bt_socket_close(fd);
                        return BT_ERR_ADDRESS;
                }
        } else {
                bacpy(&addr, &any);
        }
        if (role == BT_ROLE_LISTEN) {
                if (bt_socket_bind(fd, &addr, channel) != 0) {
                        bt_socket_close(fd);
                        return BT_ERR_BIND;
                }

                if (bt_socket_listen(fd) != 0) {
                        bt_socket_close(fd);
                        return BT_ERR_LISTEN;
                }

                listener = fd;
                fd = bt_socket_accept(listener);
                bt_socket_close(listener);
                if (fd == BT_INVALID_SOCK) {
                        return BT_ERR_ACCEPT;
                }

                bt_socket_nosigpipe(fd);
        } else if (bt_socket_connect(fd, &addr, channel) != 0) {
                bt_socket_close(fd);
                return BT_ERR_CONNECT;
        }
#elif defined(OS_WINDOWS)
        BTH_ADDR baddr = 0;
        if (role == BT_ROLE_CONNECT && !have_addr) {
                bt_socket_close(fd);
                return BT_ERR_OPTIONS;
        }

        if (have_addr) {
                if (bt_addr_from_str(addr_text, &baddr) != 0) {
                        bt_socket_close(fd);
                        return BT_ERR_ADDRESS;
                }

                if (have_addr) {
                        if (bt_addr_from_str(addr_text, &baddr) != 0) {
                                bt_socket_close(fd);
                                return BT_ERR_ADDRESS;
                        }
                } else {
                        baddr = 0;
                }

                if (role == BT_ROLE_LISTEN) {
                        if (bt_socket_bind(fd, &baddr, channel) != 0) {
                                bt_socket_close(fd);
                                return BT_ERR_BIND;
                        }

                        if (bt_socket_listen(fd) != 0) {
                                bt_socket_close(fd);
                                return BT_ERR_LISTEN;
                        }

                        listener = fd;
                        fd = bt_socket_accept(listener);
                        bt_socket_close(listener);
                        if (fd == BT_INVALID_SOCK) {
                                return BT_ERR_ACCEPT;
                        }

                        bt_socket_nosigpipe(fd);
                } else {
                        if (bt_socket_connect(fd, &baddr, channel) != 0) {
                                bt_socket_close(fd);
                                return BT_ERR_CONNECT;
                        }
                }
        }
#else
        bt_socket_close(fd);
        return BT_ERR_PLATFORM;
#endif
        st->fd = fd;
        st->role = role;
        st->ping_outstanding = 0;
        st->inited = 1;

        return BT_OK;
}

int send_chunk_bluetooth(void *conn, const struct chunk *chunk) {
        struct bt_state *st;
        int r;

        if (conn == NULL || chunk == NULL) {
                return BT_ERR_ARG;
        }

        st = (struct bt_state *)conn;
        if (st->inited != 1) {
                return BT_ERR_STATE;
        }

        r = write_frame(st->fd, chunk);
        if (r == BT_OK && chunk->type == chunk_type_ping) {
                st->ping_outstanding = 1;
        }

        return r;
}

int recv_chunk_bluetooth(void *conn, struct chunk *chunk) {
        struct bt_state *st;
        int r;

        if (conn == NULL || chunk == NULL) {
                return BT_ERR_ARG;
        }

        st = (struct bt_state *)conn;
        if (st->inited != 1) {
                return BT_ERR_STATE;
        }

        for (;;) {
                r = read_frame(st->fd, chunk);
                if (r != BT_OK) {
                        return r;
                }

                if (chunk->type != chunk_type_ping) {
                        return BT_OK;
                }

                if (st->ping_outstanding != 0) {
                        /* The answer to the ping we sent out */
                        st->ping_outstanding = 0;
                        return BT_OK;
                }

                /* write_frame directly so the ping does not set
                 * ping_outstanding and turn into our own ping */
                r = write_frame(st->fd, chunk);
                if (r != BT_OK) {
                        return r;
                }
        }
}

int close_conn_bluetooth(void *conn) {
        struct bt_state *st;

        if (conn == NULL) {
                return BT_ERR_ARG;
        }

        st = (struct bt_state *)conn;

        /*
        init_conn_bluetooth may have failed after the slot was claimed, there
        is no socket to close in that case but the caller still frees the slot
        */
        if (st->inited != 1) {
                return BT_OK;
        }

        bt_socket_close(st->fd);

        st->fd = BT_INVALID_SOCK;
        st->role = 0;
        st->ping_outstanding = 0;
        st->inited = 0;

        return BT_OK;
}

#else /* Unsupported systems */

int init_conn_bluetooth(void *conn, const char *options) {
        (void)conn;
        (void)options;
        return BT_ERR_PLATFORM;
}

int send_chunk_bluetooth(void *conn, const struct chunk *chunk) {
        (void)conn;
        (void)chunk;
        return BT_ERR_PLATFORM;
}

int recv_chunk_bluetooth(void *conn, struct chunk *chunk) {
        (void)conn;
        (void)chunk;
        return BT_ERR_PLATFORM;
}

int close_conn_bluetooth(void *conn) {
        (void)conn;
        return BT_ERR_PLATFORM;
}

#endif

const char *get_error_text_bluetooth(int code) {
        switch (code) {
        case BT_ERR_ARG:
                return "Bad argument";
        case BT_ERR_OPTIONS:
                return "Invalid options (use <channel> or <bdaddr>:<channel>)";
        case BT_ERR_STATE:
                return "Bluetooth slot already initialized";
        case BT_ERR_SOCKET:
                return "Could not create a socket";
        case BT_ERR_BIND:
                return "Could not bind to the address";
        case BT_ERR_LISTEN:
                return "Could not listen on this address";
        case BT_ERR_ACCEPT:
                return "Connection attempt failed";
        case BT_ERR_CONNECT:
                return "Could not connect to the peer";
        case BT_ERR_SEND:
                return "Send failed";
        case BT_ERR_RECV:
                return "Receive failed";
        case BT_ERR_CLOSED:
                return "The peer closed the connection";
        case BT_ERR_FRAME:
                return "Received a malformed frame";
        case BT_ERR_ADDRESS:
                return "Could not parse the bluetooth address";
        case BT_ERR_PLATFORM:
                return "Bluetooth is not supported on this system yet";
        default:
                return "Unknown bluetooth error";
        }
}
