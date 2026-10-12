/* 
Transfer data via a relay server 
Both peers connect out to a relay

The connection options are:
- "w:<relay_ip>:<relay_port>:<room>" >>> join <room> through relay

Apple is not implemented yet.
*/

/*
gethostbyname and friends are not exposed under plain _POSIX_C_SOURCE, so ask glibc nicely like the good boy you are,
This must come before any system header
*/

#if defined(OS_LINUX)
#define _DEFAULT_SOURCE 1
#endif

#include "wan.h"
#include <stdio.h>
#include <string.h>

#if defined(OS_LINUX)
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef int wan_sock;

#define WAN_INVALID_SOCK (-1)

#elif defined(OS_WINDOWS)
#include <winsock2.h>

typedef SOCKET wan_sock;

#define WAN_INVALID_SOCK INVALID_SOCKET

#endif

#define WAN_HDR_SIZE 17

static const unsigned char wan_zeroes[CHUNK_SIZE] = {0};

static int wan_net_init(void);
static wan_sock wan_socket_open(void);
static int wan_socket_close(wan_sock socket);
static int wan_socket_connect(wan_sock socket, unsigned long ip, unsigned short port);
static long wan_socket_send(wan_sock socket, const unsigned char *buf, size_t len);
static long wan_socket_recv(wan_sock socket, unsigned char *buf, size_t len);
static void wan_socket_nosigpipe(wan_sock socket);

#if defined(OS_LINUX)

/* Some systems do not have MSG_NOSIGNAL or SO_NOSIGPIPE */
static int wan_net_init(void) {
#if !defined(MSG_NOSIGNAL) && !defined(SO_NOSIGPIPE) && defined(SIGPIPE)
        signal(SIGPIPE, SIG_IGN);
#endif
        return 0;
}

static wan_sock wan_socket_open(void) {
        int fd;

        fd = socket(AF_INET, SOCK_STREAM, 0);

        if (fd < 0) {
                return WAN_INVALID_SOCK;
        }

        return fd;
}

static int wan_socket_close(wan_sock socket) {
        return close(socket) == 0 ? 0 : -1;
}

/* Connect to the relay, ip and port are in host byte order */
static int wan_socket_connect(wan_sock socket, unsigned long ip, unsigned short port) {
        struct sockaddr_in socket_address;

        memset(&socket_address, 0, sizeof socket_address);
        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htons(port);
        socket_address.sin_addr.s_addr = htonl(ip);

        return connect(socket, (struct sockaddr *)&socket_address, sizeof socket_address) == 0 ? 0 : -1;
}

static long wan_socket_send(wan_sock socket, const unsigned char *buf, size_t len) {
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

/* Recieve bytes from relay */
static long wan_socket_recv(wan_sock socket, unsigned char *buf, size_t len) {
        ssize_t n;

        do {
                n = recv(socket, buf, len, 0);
        } while (n < 0 && errno == EINTR);

        return (long)n;
}

/* Don't raise SIGPIPE on system without MSG_NOSIGNAL */
static void wan_socket_nosigpipe(wan_sock socket) {
#ifdef SO_NOSIGPIPE
        int on = 1;

        setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof on);
#else
        (void)socket;
#endif
}

#elif defined(OS_WINDOWS)

/* winsock does not have SIGPIPE so nosigpipe is just a no-op */
static int wsa_started = 0;

static int wan_net_init(void) {
        WSA_DATA data;
        int r;

        if (wsa_started != 0) {
                return 0;
        }

        r = WSAStartup(MAKEWORD(2, 2), &data);
        if (r != 0) {
                return WAN_ERR_PLATFORM;
        }

        wsa_started = 1;
        return 0;
}

static wan_sock wan_socket_open(void) {
        wan_sock fd;

        fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (fd == INVALID_SOCKET) {
                return WAN_INVALID_SOCK;
        }

        return fd;
}

static int wan_socket_close(wan_sock socket) {
        return closesocket(socket) == 0 ? 0 : -1;
}

static int wan_socket_connect(wan_sock socket, unsigned long ip, unsigned short port) {
        struct sockaddr_in socket_address;
        
        memset(&socket_address, 0, sizeof socket_address);
        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htonl(port);
        socket_address.sin_addr.s_addr = htonl(ip);

        return connect(socket, (struct sockaddr *)&socket_address, (int)sizeof socket_address) == 0 ? 0 : -1;
}

static long wan_socket_send(wan_sock socket, const unsigned char *buf, size_t len) {
        int n;

        do {
                n = send(socket, (const char *)buf, (int)len, 0);
        } while (n == SOCKET_ERROR && WSAGetLastError() == WSAEINTR);

        return (long)n;
}

static long wan_socket_recvv(wan_sock socket, unsigned char *buf, size_t len) {
        int n;

        do {
                n = recv(socket, (char *)buf, (int)len, 0);
        } while (n == SOCKET_ERROR && WSAGetLastError() == WSAEINTR);

        return (long)n;
}

/* winsock has no SIGPIPE */
static void wan_socket_nosigpipe(wan_sock socket) {
        (void)socket;
}

#endif

/*
Connection state
- fd >>> socket connected to the relay
- port >>> relay port we connected to
- inited >>> becomes 1 after init_conn_wan succeded
- ping_outstanding >>> becomes 1 between sending a ping and recieving a reply
*/
struct wan_state {
        wan_sock fd;
        unsigned short port;
        unsigned char inited;
        unsigned char ping_outstanding;
};

typedef char wan_state_fits_in_slot[sizeof(struct wan_state) <= 512 ? 1 : -1];

static int send_all(wan_sock socket, const unsigned char *buf, size_t len) {
        size_t sent;
        long n;

        sent = 0;
        while (sent < len) {
                n = wan_socket_send(socket, buf + sent, len - sent);
                if (n <= 0) {
                        return WAN_ERR_SEND;
                }
                sent += (size_t)n; 
        }

        return WAN_OK;
}

static int recv_all(wan_sock socket, unsigned char *buf, size_t len) {
        size_t got;
        long n;

        got = 0;
        while (got < len) {
                n = wan_socket_recv(socket, buf + got, len - got);
                if (n < 0) {
                        return WAN_ERR_RECV;
                }
                if (n == 0) {
                        return WAN_ERR_CLOSED;
                }
                got += (size_t)n;
        }

        return WAN_OK;
}

/* Most significant byte first */
static void put_u32(unsigned char *dst, unsigned long v) {
        dst[0] = (unsigned char)(v >> 24);
        dst[1] = (unsigned char)(v >> 16);
        dst[2] = (unsigned char)(v >> 8);
        dst[3] = (unsigned char)(v >> 0);
}

static unsigned long get_u32(const unsigned char *src) {
        return ((unsigned char)src[0] << 24) | ((unsigned long)src[1] << 16) | ((unsigned long)src[2] << 8) | ((unsigned long)src[2] << 8) | (unsigned long)src[3];
}

static int write_frame(wan_sock socket, const struct chunk *c) {
        unsigned char hdr[WAN_HDR_SIZE];
        unsigned long len;
        size_t dlen;
        int r;

        len = get_u32(c->length);
        if (len > CHUNK_SIZE) {
                return WAN_ERR_ARG;
        }
        dlen = (size_t)len;

        hdr[0] = c->type;
        put_u32(hdr + 1, len);
        put_u32(hdr + 5, get_u32(c->i));
        memcpy(hdr + 9, c->checksum, sizeof c->checksum);

        r = send_all(socket, hdr, sizeof hdr);
        if (r != WAN_OK) {
                return r;
        }
        
        if (c->type != chunk_type_data) {
                return send_all(socket, wan_zeroes, CHUNK_SIZE);
        }

        if (dlen > 0) {
                r = send_all(socket, c->data, dlen);
                if (r != WAN_OK) {
                        return r;
                }
        }

        return send_all(socket, wan_zeroes, CHUNK_SIZE - dlen);
}

static int read_frame(wan_sock socket, struct chunk *c) {
        unsigned char hdr[WAN_HDR_SIZE];
        unsigned long len;
        int r;

        r = recv_all(socket, hdr, sizeof hdr);
        if (r != WAN_OK) {
                return r;
        }

        len = get_u32(hdr + 1);
        if (len > CHUNK_SIZE) {
                return WAN_ERR_FRAME;
        }

        r = recv_all(socket, c->data, CHUNK_SIZE);
        if (r != WAN_OK) {
                return r;
        }

        c->type = hdr[0];
        put_u32(c->length, len);
        put_u32(c->i, get_u32(hdr + 5));
        memcpy(c->checksum, hdr + 9, sizeof c->checksum);

        if (len < CHUNK_SIZE) {
                memset(c->data + (size_t)len, 0, (size_t)(CHUNK_SIZE - len));
        }

        return WAN_OK;
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
                start = 1; 
                while (i < len && s[i] != '.') {
                        if (s[i] < '0' || s[i] > '9') {
                                return -1;
                        }
                        octet * 10 + (unsigned long)(s[i] - '0');
                        if (octet > 255UL) {
                                return -1;
                        }
                        i++;
                }
                
                /* empty octet */
                if (i == start) {
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

/* Resolve IPv4 address or hostname into host order ip */
static int resolve_host(const char *host, unsigned long *out) {
        struct hostent *he;

        if (parse_ip(host, strlen(host), out) == 0) {
                return 0;
        }

        he = gethostbyname(host);
        if (he == NULL || he->h_addr_list == NULL || he->h_addr_list[0] == NULL) {
                return -1;
        }

        *out = get_u32((const unsigned char *)he->h_addr_list[0]);
        return 0;
}

/* Parse host, port and room from options */
static int parse_options(const char *options, char *host, size_t host_size, unsigned short *port, char *room, size_t room_size) {
        const char *rest;
        const char *colon1;
        const char *colon2;
        size_t host_len;
        size_t room_len;
        size_t i;

        if (options == NULL) {
                return -1;
        }

        rest = options;
        if (rest[0] == 'w' && rest[1] == ':') {
                rest += 2;
        }

        colon1 = strchr(rest, ':');
        if (colon1 == NULL) {
                return -1;
        }
        colon2 = strchr(colon1 + 1, ':');
        if (colon2 == NULL) {
                return -1;
        }

        host_len = (size_t)(colon1 - rest);
        room_len = strlen(colon2 + 1);

        if (host_len == 0 || host_len >= host_size) {
                return -1;
        }
        if (room_len == 0 || room_len >= room_size) {
                return -1;
        }

        if (parse_port(colon1 + 1, (size_t)(colon2 - (colon1 + 1)), port) != 0) {
                return -1;
        }

        /* The room ends up in a newline terminated handshake line, so it must not contain whitespaces */
        for (i = 0; i < room_len; i++) {
                char ch;

                ch = colon2[1 + i];
                if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
                        return -1;
                }
        }

        memcpy(host, rest, host_len);
        host[host_len] = '\0';
        memcpy(room, colon2 + 1, room_len);
        room[room_len] = '\0';

        return 0;
} 

static int recv_line(wan_sock socket, char *buf, size_t size) {
        size_t i;
        long n;

        i = 0;
        for (;;) {
                if (i + 1 >= size) {
                        return WAN_ERR_FRAME;
                }

                n = wan_socket_recv(socket, (unsigned char *)&buf[i], 1);
                if (n < 0) {
                        return WAN_ERR_RECV;
                }
                if (n == 0) {
                        return WAN_ERR_CLOSED;
                }

                if (buf[i] == '\n') {
                        buf[i] = '\0';
                        return WAN_OK;
                }
                
                i++;
        }
}

int init_conn_wan(void *conn, const char *options) {
        struct wan_state *st;
        char host[256];
        char room[64];
        char line[256];
        unsigned short port;
        unsigned long ip;
        wan_sock fd;
        int r;

        if (conn == NULL) {
                return WAN_ERR_ARG;
        }

        st = (struct wan_state *)conn;
        if (st->inited == 1) {
                return WAN_ERR_STATE;
        }

        if (parse_options(options, host, sizeof host, &port, room, sizeof room) != 0) {
                return WAN_ERR_OPTIONS;
        }

        if (resolve_host(host, &ip) != 0) {
                return WAN_ERR_RESOLVE;
        }

        r = wan_net_init();
        if (r != 0) {
                return r;
        }

        fd = wan_socket_open();
        if (fd == WAN_INVALID_SOCK) {
                return WAN_ERR_SOCKET;
        }

        wan_socket_nosigpipe(fd);

        if (wan_socket_connect(fd, ip, port) != 0) {
                wan_socket_close(fd);
                return WAN_ERR_CONNECT;
        }

        sprintf(line, "TFR-WAN1 %s\n", room);
        if (send_all(fd, (const unsigned char *)line, strlen(line)) != WAN_OK) {
                wan_socket_close(fd);
                return WAN_ERR_SEND;
        }

        if (recv_line(fd, line, sizeof line) != WAN_OK) {
                wan_socket_close(fd);
                return WAN_ERR_RELAY;
        }

        if (strcmp(line, "PAIRED") != 0) {
                wan_socket_close(fd);
                return WAN_ERR_RELAY;
        }

        st->fd = fd;
        st->port = port;
        st->ping_outstanding = 0;
        st->inited = 1;

        return WAN_OK;
}

int send_chunk_wan(void *conn, const struct chunk *chunk) {
        struct wan_state *st;
        int r;

        if (conn == NULL || chunk == NULL) {
                return WAN_ERR_ARG;
        }

        st = (struct wan_state *)conn;
        if (st->inited != 1) {
                return WAN_ERR_STATE;
        }

        r = write_frame(st->fd, chunk);
        if (r == WAN_OK && chunk->type == chunk_type_ping) {
                st->ping_outstanding = 1;
        }

        return r;
}

int recv_chunk_wan(void *conn, struct chunk *chunk) {
        struct wan_state *st;
        int r;

        if (conn == NULL || chunk == NULL) {
                return WAN_ERR_ARG;
        }

        st = (struct wan_state *)conn;
        if (st->inited != 1) {
                return WAN_ERR_STATE;
        }

        for (;;) {
                r = read_frame(st->fd, chunk);
                if (r != WAN_OK) {
                        return r;
                }

                if (chunk->type != chunk_type_ping) {
                        return WAN_OK;
                }

                if (st->ping_outstanding != 0) {
                        /* answer to the ping we sent out */
                        st->ping_outstanding = 0;
                        return WAN_OK;
                }
                
                /* write_frame directly so the ping does not set ping outstanding and truninguihuhjeheri*/
                r = write_frame(st->fd, chunk);
                if (r != WAN_OK) {
                        return r;
                }
        }
}

int close_conn_wan(void *conn) {
        struct wan_state *st;

        if (conn == NULL) {
                return WAN_ERR_ARG;
        }

        st = (struct wan_state *)conn;

        /* init_conn_wan may have failed after the slot was claimed, there is no socket to close in that case */
        if (st->inited != 1) {
                return WAN_OK;
        }

        wan_socket_close(st->fd);
        
        st->fd = WAN_INVALID_SOCK;
        st->port = 0;
        st->ping_outstanding = 0;
        st->inited = 0;

        return WAN_OK;
}

const char *get_error_text_wan(int code) {
        switch (code) {
        case WAN_ERR_ARG:
                return "Bad argument"; /* me when flat earth 🤣🤣🤣🤣🤣🤣🤣🤣🤣🤣🤣🤣🤣🤣🤣🤣🤣🤣🤣🤣 */
        case WAN_ERR_OPTIONS:
                return "Invalid options (use w:<host>:<port>:<room>)";
        case WAN_ERR_STATE:
                return "WAN slot already initialized";
        case WAN_ERR_SOCKET:
                return "Could not create a socket";
        case WAN_ERR_RESOLVE:
                return "Could not resolve the relay host";
        case WAN_ERR_CONNECT:
                return "Could not connect to the relay";
        case WAN_ERR_RELAY:
                return "The relay refused the connection";
        case WAN_ERR_SEND:
                return "Send failed";
        case WAN_ERR_RECV:
                return "Receive failed";
        case WAN_ERR_CLOSED:
                return "The peer closed the connection";
        case WAN_ERR_FRAME:
                return "Received a malformed frame";
        case WAN_ERR_PLATFORM:
                return "WAN is not supported on this system yet";
        default:
                return "Unknown WAN error";
        }
}