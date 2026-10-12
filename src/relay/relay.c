/*
tiny relay server 
run 
peer conects to relay and other peer also does
relay copies bytes from peer to other peer
*/

#if defined(OS_LINUX)
#define _DEFAULT_SOURCE 1
#endif

#include "relay.h"
#include <stdio.h>
#include <string.h>

#if defined(OS_LINUX)
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/sockets.h>
#include <sys/types.h>
#include <unistd.h>

typedef int relay_sock;

#define RELAY_INVALID_SOCK (-1)

#elif defined(OS_WINDOWS)
#include <winsock2.h>

typedef SOCKET relay_sock;

#define RELAY_INVALID_SOCK INVALID_SOCKET

#endif

#define RELAY_MAX_CLIENTS 32
#define RELAY_ROOM_MAX 63
#define RELAY_LINE_MAX 128
#define RELAY_OUTBUF 16384

#define RELAY_ST_HANDSHAKE 0
#define RELAY_ST_WAITING 1
#define RELAY_ST_PAIRED 2

/*
Each client has an output buffer holding bytes that are destined for its peer but could not be written yet.
*/
struct relay_client {
        relay_sock fd;
        int used;
        int state;
        int peer;
        int closing;
        char room[RELAY_ROOM_MAX + 1];
        char line[RELAY_LINE_MAX];
        int linelen;
        unsigned char out[RELAY_OUTBUF];
        size_t outlen;
        size_t outoff;
};

static struct relay_client clients[RELAY_MAX_CLIENTS];

static int relay_net_init(void);
static relay_sock relay_socket_open(void);
static int relay_socket_close(relay_sock socket);
static int relay_socket_reuseaddr(relay_socket socket);
static int relay_socket_bind(relay_sock socket, unsigned long ip, unsigned short port);
static int relay_socket_listen(relay_sock socket);
static relay_sock relay_socket_accept(relay_sock socket);
static long relay_socket_send(relay_sock socket, const unsigned char *buf, size_t len);
static long relay_socket_recv(relay_sock socket, unsigned char *buf, size_t len);
static void relay_socket_nosigpipe(relay_sock socket);

#if defined(OS_LINUX)

static int relay_net_init(void) {
#if !defined(MSG_NOSIGNAL) && !defined(SO_NOSIGPIPE) && defined(SIGPIPE)
        signal(SIGPIPE, SIG_IGN)
#endif
        return 0;
}

static relay_sock relay_socket_open(void) {
        int fd;

        fd = socket(AF_INET, SOCK_STREAM, 0);

        if (fd < 0) {
                return RELAY_INVALID_SOCK;
        }

        return fd;
}

static int relay_socket_close(relay_sock socket) {
        return close(socket) == 0 ? 0 : -1;
}

static int relay_socket_reuseaddr(relay_sock socket) {
        int on = 1;

        return setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on) == 0 ? 0 : -1;
}

static int relay_socket_bind(relay_sock socket, unsigned long ip, unsigned short port) {
        struct sockaddr_in socket_address;

        memset(&socket_address, 0, sizeof socket_address);
        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htons(port);
        socket_address.sin_addr.s_addr = htonl(ip);

        return bind(socket, (struct sockaddr *)&socket_address, sizeof socket_address) == 0 ? 0 : -1;
}

static int relay_socket_listen(relay_sock socket) {
        return listen(socket, RELAY_MAX_CLIENTS) == 0 ? 0 : -1;
}

static relay_sock relay_socket_accept(relay_sock socket) {
        int fd;

        fd = accept(socket, NULL, NULL);

        if (fd < 0) {
                return RELAY_INVALID_SOCK;
        }

        return fd;
}

static long relay_socket_send(relay_sock socket, const unsigned char *buf, size_t len) {
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

static long relay_socket_recv(relay_sock socket, unsigned char *buf, size_t len) {
        ssize_t n;

        do {
                n = recv(socket, buf, len, 0);
        } while (n < 0 && errno == EINTR);

        return (long)n;
}

static void relay_socket_nosigpipe(relay_sock socket) {
#ifdef SO_NOSIGPIPE
        int on = 1;

        setsockopt(socket, SOL_SOCKET, SO_NOSIGPIPE, &on, sizeof on);
#else
        (void)socket;
#endif
}

#elif defined(OS_WINDOWS)

static int wsa_started = 0;

static int relay_net_init(void) {
        WSADATA data;
        int r;

        if (wsa_started != 0) {
                return 0;
        }

        wsa_started = 1;
        return 0;
}

static relay_sock relay_socket_open(void) {
        relay_sock fd;

        fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (fd == INVALID_SOCKET) {
                return RELAY_INVALID_SOCK;
        }

        return fd;
}

static int relay_socket_cloe(relay_sock socket) {
        return closesocket(socket) == 0 ? 0 : -1;
}

static int relay_socket_reuseaddr(relay_sock socket) {
        int on = 1;

        return setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, (int)sizeof on) == 0 ? 0 : -1;
}

static int relay_socket_bind(relay_sock socket, unsigned long ip, unsigned short port) {
        struct sockaddr_in socket_address;
        
        memset(&socket_address, 0, sizeof socket_address);
        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htons(port);
        socket_address.sin_addr.s_addr = htonl(ip);

        return bind(socket, (struct sockaddr *)&socket_address, (int)sizeof socket_address) == 0 ? 0 : -1;
}

static int relay_socket_listen(relay_sock socket) {
        return listen(socket, RELAY_MAX_CLIENTS) == 0 ? 0 : -1;
}

static relay_sock relay_socket_accept(relay_sock socket) {
        relay_sock fd;

        fd = accept(socket, NULL, NULL);
        if (fd == INVALID_SOCKET) {
                return RELAY_INVALID_SOCK;
        }

        return fd;
}

static long relay_socket_send(relay_sock socket, const unsigned char *buf, size_t len) {
        int n;

        do {
                n = send(socket, (const char *)buf, (int)len, 0);
        } while (n == SOCKET_ERROR && WSAGetLastError() == WSAEINTR);

        return (long)n;
}

static long relay_socket_recv(relay_sock socket, unsigned char *buf, size_t len) {
        int n;
        
        do {
                n = recv(socket, (char *)buf, (int)len, 0);
        } while (n == SOCKET_ERROR && WSAGetLastError() == WSAEINTR);

        return (long)n;
}

static void relay_socket_nosigpipe(relay_sock socket) {
        (void)socket;
}

#endif

static int parse_port(const char *s, unsigned short *out) {
        unsigned long v;
        size_t i;
        size_t len;

        if (s == NULL) {
                return -1;
        }

        len = strlen(s);
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

/* Queue bytes for client, compacting buffer first */
static int relay_enqueue(struct relay_client *c, const unsigned char *data, size_t len) {
        if (c->outoff > 0) {
                if (c->outoff < c->outlen) {
                        memmove(c->out, c->out + c->outoff, c->outlen - c->outoff);
                }
                c->outlen -= c->outoff;
                c->outoff = 0;
        }

        if (c->outlen + len > RELAY_OUTBUF) {
                return -1;
        }

        memcpy(c->out + c->outlen, data, len);
        c->outlen += len;
        
        return 0;
}

static int relay_alloc_client(relay_sock fd){
        int i;
        
        for (i = 0; i < RELAY_MAX_CLIENTS; i++) {
                if (!clients[i].used) {
                        memset(&clients[i], 0, sizeof clients[i]);
                        clients[i].used = 1;
                        clients[i].fd = fd;
                        clients[i].peer = -1;

                        return i;
                }
        }

        return -1;
}

/* Close a client and ask it's peer to flush whatever is left and close too*/
static void relay_close_client(int i) {
        int peer;

        if (!clients[i].used) {
                return;
        }

        peer = clients[i].peer;

        relay_socket_close(clients[i].fd);
        clients[i].used = 0;
        clients[i].fd = RELAY_INVALID_SOCK;        
        clients[i].peer = -1;

        if (peer >= 0 && clients[peer].used) {
                clients[peer].closing = 1;
                clients[peer].peer = -1;
        }        
}

static void relay_send_error(struct relay_client *c, const char *reason) {
        size_t len;
        char buf[RELAY_LINE_MAX];

        sprintf(buf, "ERR %s\n", reason);
        len = strlen(buf);
        relay_socket_send(c->fd, (const unsigned char *)buf, len);
}

static int relay_find_waiting(const char *room, int exclude) {
        int i;

        for (i = 0; i < RELAY_MAX_CLIENT; i++) {
                if (i == exclude || !clients[i].used) {
                        continue;
                }
                if (clients[i].state == RELAY_ST_WAITING && strcmp(clients[i].room, room) == 0){
                        return i;
                }
        }

        return -1;
}

/* Handshake arrived for client, what to do? */
static void relay_handle_handshake(int i) {
        struct relay_client *c;
        int j; // no i :<
        char paired[] = "PAIRED\n";

        c = &clients[i];
        
        if (strncmp(c->line, "TFR-WAN1 ", 9) != 0 || c->line[9] == '\0') {
                relay_send_error(c, "bad handshake"); // ok so this was going to be a joke but i can't think of it
                relay_close_client(i);
                return;
        }

        strncpy(c->room, c->line + 9, RELAY_ROOM_MAX);
        c->room[RELAY_ROOM_MAX] = '\0';

        j = relay_find_waiting(c->room, i);
        if (j < 0) {
                c->state = RELAY_ST_WAITING;
                return;
        }

        c->state = RELAY_ST_PAIRED;
        c->peer = j;
        clients[j].state = RELAY_ST_PAIRED;
        clients[j].peer = i;

        relay_enqueue(c, (const unsigned char *)paired, strlen(paired));
        relay_enqueue(&clients[j], (const unsigned char *)paired, strlen(paired));
}

int run_relay(const char *port_str) {
        unsigned short port;
        relay_sock listener;
        relay_sock maxfd;
        int i;
        int n;
        char buf[4096];
        long got;

        if (parse_port(port_str, &port) != 0) {
                fprintf(stderr, "relay: invalid port '%s'\n", port_str);
                return 1;
        }

        if (relay_net_init() != 0) {
                fprintf(stderr, "relay: could not initialize networking\n");
                return 1;
        }

        listener = relay_socket_open();
        if (listener == RELAY_INVALID_SOCK) {
                fprintf(stderr, "relay: could not create a socket\n");
                return 1;
        }

        relay_socket_nosigpipe(listener);
        relay_socket_reuseaddr(listener);

        if (relay_socket_bind(listener, 0, port) != 0) {
                fprintf(stderr, "relay: could not bind to port %u\n", port);
                relay_socket_close(listener);
                return 1;
        }

        if (relay_socket_listen(listener) != 0) {
                fprintf(stderr, "relay: could not listen on port %u\n", port);
                relay_socket_close(listener);
                return 1;
        }

        fprintf(stderr, "relay: listening on port %u\n", port);
        for (;;) {
                fd_set rfds;
                fd_set wfds;

                FD_ZERO(&rfds);
                FD_ZERO(&wfds);
                FD_SET(listener, &rfds);
                maxfd = listener;

                for (i = 0; i < RELAY_MAX_CLIENTS; i++) {
                        struct relay_client *c;

                        c = &clients[i];
                        if (!c->used) {
                                continue;
                        }

                        if (c->fd > maxfd) {
                                maxfd = c->fd;
                        }

                        if (c->state == RELAY_ST_HANDSHAKE || c->state == RELAY_ST_WAITING) {
                                FD_SET(c->fd, &rfds);
                        } else if (c->state == RELAY_ST_PAIRED && c->peer >= 0 && clients[c->peer].used && !client[c->peer].closing && clients[c->peer].outlen < RELAY_OUTBUF) {
                                FD_SET(c->fd, &rfds);
                        } 

                        if (c->outoff < c->outlen) {
                                FD_SET(c->fd, &wfds);
                        }
                }

                n = select((int)maxfd + 1, &rfds, &wfds, NULL, NULL);
                if (n < 0) {
                        /* interrupted, go around again */
                        continue;
                }

                if (FD_ISSET(listener, &rfds)) {
                        relay_sock fd;

                        fd = relay_socket_accept(listener);
                        if (fd != RELAY_INVALID_SOCK) {
                                relay_socket_nosigpipe(fd);
                                if (relay_alloc_client(fd) < 0) {
                                        relay_socket_close(fd);
                                }
                        }
                }

                for (i = 0; i < RELAY_MAX_CLIENTS; i++) {
                        struct relay_client *c;

                        c = &clients[i];
                        if (!c->used) {
                                continue;
                        }

                        if (FD_ISSET(c->fd, &wfds)) {
                                got = relay_socket_send(c->fd, c->out + c->outoff, c->outlen - c->outoff);
                                if (got <= 0) {
                                        relay_close_client(i);
                                        continue;
                                }
                                c->outoff += (size_t)got;
                                if (c->outoff == c->outlen) {
                                        c->outoff = 0;
                                        c->outlen = 0;
                                }
                                if (c->closing && c->outoff == c->outlen) {
                                        relay_close_client(i);
                                        continue;
                                }
                        }

                        if (!FD_ISSET(c->fd, rfds)) {
                                continue;
                        }

                        if (c->state == RELAY_ST_HANDSHAKE) {
                                size_t space;

                                space = (size_t)(RELAY_LINE_MAX - 1 - c->linelen);
                                got = relay_socket_recv(c-> fd, (unsigned char *)(c->line + c->linelen), space);

                                if (got <= 0) {
                                        relay_close_client(i);
                                } else {
                                        c->linelen += (int)got;
                                        c->line[c->linelen] = '\0';
                                        if (strchr(c->line, '\n') != NULL) {
                                                relay_handle_handshake(i);
                                        } else if (c->state == RELAY_ST_WAITING) {
                                                got = relay_socket_recv(c->fd, (unsigned char *)buf, sizeof buf);
                                                if (got <= 0) {
                                                        relay_close_client(i);
                                                        continue;
                                                }
                                                if (peer >= 0 && clients[peer].used) {
                                                        relay_enqueue(&clients[peer], (unsigned char *)buf, (size_t)got);
                                                }
                                        }
                                }
                        }
                }
        }

        return 0;
}