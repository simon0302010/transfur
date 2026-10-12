/*
Transfer data over Bluetooth RFCOMM

The connection options are:
- "l:<channel>" >>> listen on <channel> on all local adapters and wait for a peer to connect
- "l:<bdaddr>:<channel>" >>> listen to one specific adapter
- "c:<bdaddr>:<channel>" >>> connect to peer

Apple and Windows are not implemented
*/

#include "bluetooth.h"
#include <string.h>

#if defined(OS_LINUX)

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef int bt_sock;

#define BT_INVALID_SOCK (-1)
#define BT_HDR_SIZE 17

static const unsigned char bt_zeroes[CHUNK_SIZE] = {0};

static int bt_net_init(void);
static bt_sock bt_socket_open(void);
static int bt_socket_close(bt_sock socket);
static int bt_socket_bind(bt_sock socket, const bdaddr_t *addr, unsigned int channel);
static int bt_socket_listen(bt_sock socket);
static bt_sock bt_socket_accept(bt_sock socket);
static int bt_socket_connect(bt_sock socket, const bdaddr_t *addr, unsigned int channel);
static long bt_socket_send(bt_sock socket, const unsigned char *buf, size_t len);
static long bt_socket_recv(bt_sock socket, unsigned char *buf, size_t len);
static void bt_socket_nosigpipe(bt_sock socket);

/* BT RFCOMM does not raise SIGPIPE I think but idk man */
static int bt_net_init(void) {
#if !defined(MSG_NOSIGNAL) && !defined(SO_NOSIGPIPE) && defined(SIGPIPE)
        signal(SIGPIPE, SIG_IGN);
#endif
        return 0;
}

static bt_sock bt_socket_open(void) {
        int fd;

        fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

        if (fd < 0) {
                return BT_INVALID_SOCK;
        }

        return fd;
}

static int bt_socket_close(bt_sock socket) {
        return close(socket) == 0 ? 0 : -1;
}

/* Bind a listening socket, addr = BDADDR_ANY binds to all adapters */
static int bt_socket_bind(bt_sock socket, const bdaddr_t *addr, unsigned int channel) {
        struct sockaddr_rc socket_address;

        memset(&socket_address, 0, sizeof socket_address);
        socket_address.rc_family = AF_BLUETOOTH;
        socket_address.rc_bdaddr = *addr;
        socket_address.rc_channel = (unsigned char)channel;

        return bind(socket, (struct sockaddr *)&socket_address, sizeof socket_address) == 0 ? 0 : -1;
}

static int bt_socket_listen(bt_sock socket) {
        return listen(socket, MAX_CONN) == 0 ? 0 : -1;
}

static bt_sock bt_socket_accept(bt_sock socket) {
        int fd;

        fd = accept(socket, NULL, NULL);

        if (fd < 0) {
                return BT_INVALID_SOCK;
        }

        return fd;
}

/* Connect to Mister Peer */
static int bt_socket_connect(bt_sock socket, const bdaddr_t *addr, unsigned int channel) {
        struct sockaddr_rc socket_address;

        memset(&socket_address, 0, sizeof socket_address);
        socket_address.rc_family = AF_BLUETOOTH;
        socket_address.rc_bdaddr = *addr;
        socket_address.rc_channel = (unsigned char)channel;

        return connect(socket, (struct sockaddr *)&socket_address, sizeof socket_address) == 0 ? 0 : -1;
}

/* Send bytes to peer */
static long bt_socket_send(bt_sock socket, const unsigned char *buf, size_t len) {
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
static long bt_socket_recv(bt_sock socket, unsigned char *buf, size_t len) {
        ssize_t n;

        do {
                n = recv(socket, buf, len, 0);
        } while (n < 0 && errno == EINTR);

        return (long)n;
}

static void bt_socket_nosigpipe(bt_sock socket) {
#ifdef SO_NOSIGPIPE
        int on = 1;

        setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof on);
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
        return ((unsigned long)src[0] << 24) | ((unsigned long)src[1] << 16) | ((unsigned long)src[2] << 8) | (unsigned long)src[3];
}

/*TODO: Finish this because I am eepy right now*/