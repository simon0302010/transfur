#include "discover.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../misc/ttime.h"

#if defined(OS_LINUX)

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef int discover_sock;

#define DISCOVER_INVALID_SOCK (-1)

#elif defined(OS_WINDOWS)
#include <winsock2.h>

typedef SOCKET discover_sock;

#define DISCOVER_INVALID_SOCK INVALID_SOCK

#endif

#define DISCOVER_PORT 9001
#define DISCOVER_SCAN_SECONDS 2.0
#define DISCOVER_PROBE_SECONDS 0.75
#define DISCOVER_QUERY "TFDISCv1 QUERY "
#define DISCOVER_REPLY "TFDISCv1 REPLY "
#define DISCOVER_PREFIX_LEN 15 /* strlen of either query/reply above */
#define DISCOVER_ID_LEN 4
#define DISCOVER_MAX_FRAME 64

static discover_sock listen_sock = DISCOVER_INVALID_SOCK;
static discover_sock scan_sock = DISCOVER_INVALID_SOCK;
static char id[DISCOVER_ID_LEN + 1] = "";
static int listener_port = 0;
static int init_attempted = 0;
static int init_ready = 0;

static int scanning = 0;
static double scan_deadline = 0.0;
static double last_probe = 0.0;

static char peer_ip[MAX_PEERS][16];
static int peer_port[MAX_PEERS];
static int peer_count = 0;

static void discover_close_sock(discover_sock sock);
static int discover_open(discover_sock *out, unsigned short port);
static int discover_wait(discover_sock sock, int timeout_ms);
static long discover_recv(discover_sock sock, char *buf, int buf_size, struct sockaddr_in *from);
static void discover_send_probe(discover_sock sock);
static void discover_send_reply(discover_sock sock, const char *reply_id, const struct sockaddr_in *to);
static void discover_drain(discover_sock sock);
static void discover_add_peer(const char *ip, int port);

static void discover_close_sock(discover_sock sock) {
#if defined(OS_WINDOWS) 
        (void)closesocket(sock);
#else  
        (void)close(sock);
#endif
}

/* UDP socket bound to `port` (0 = any free port) that accepts broadcasts */
static int discover_open(discover_sock *out, unsigned short port) {
        discover_sock sock;
        struct sockaddr_in addr;
        int on = 1;

        sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock == DISCOVER_INVALID_SOCK) {
                return -1;
        }

        (void)setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof on);
        (void)setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char *)&on, sizeof on);

        memset(&addr, 0, sizeof addr);
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(sock, (struct sockaddr *)&addr, sizeof addr) != 0) {
                discover_close_sock(sock);
                return -1;
        }

        *out = sock;
        return 0;
}

/* is a datagram waiting? return `0` when nothing arrived */
static int discover_wait(discover_sock sock, int timeout_ms) {
        fd_set rfds;
        struct timeval tv;
        int r;

        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;

#if defined(OS_WINDOWS)
        r = select(0, &rfds, NULL, NULL, &tv);
#else
        r = select(sock + 1, &rfds, NULL, NULL, &tv);
#endif
        if (r <= 0) {
                return 0;
        }

        return FD_ISSET(sock, &rfds) ? 1 : 0;
}

/* Read one datagram and null-terminate it */
static long discover_recv(discover_sock sock, char *buf, int buf_size, struct sockaddr_in *from) {
#if defined(OS_WINDOWS) 
        int fromlen;
#else
        socklen_t fromlen;
#endif
        long n;

        fromlen = (int)sizeof *from;
        n = (long)recvfrom(sock, buf, (size_t)buf_size - 1, 0, (struct sockaddr *)from, &fromlen);

        if (n < 0) {
                return -1;
        }

        buf[n] = '\0';

        return n;
}

static void discover_send_probe(discover_sock sock) {
        char buf[DISCOVER_MAX_FRAME];
        struct sockaddr_in to;

        sprintf(buf, "%s%s", DISCOVER_QUERY, id);

        memset(&to, 0, sizeof to);
        
        to.sin_family = AF_INET;
        to.sin_port = htons(DISCOVER_PORT);
        to.sin_addr.s_addr = htonl(INADDR_BROADCAST);

        (void)sendto(sock, buf, (int)strlen(buf), 0, (struct sockaddr *)&to, sizeof to);
} 

static void discover_send_reply(discover_sock sock, const char *reply_id, const struct sockaddr_in *to) {
        char buf[DISCOVER_MAX_FRAME];

        sprintf(buf, "%s%s %d", DISCOVER_REPLY, reply_id, listener_port);

        (void)sendto(sock, buf, (int)strlen(buf), 0, (const struct sockaddr *)to, sizeof *to);
}

/* Throw away stale datagrams from earlier scans */
static void discover_drain(discover_sock sock) {
        char buf[DISCOVER_MAX_FRAME];
        struct sockaddr_in from;
        int i;

        for (i = 0; i < 32; i++) {
                if (!discover_wait(sock, 0)) {
                        break;
                }
                if (discover_recv(sock, buf, sizeof buf, &from) < 0) {
                        break;
                }
        }
}

static void discover_add_peer(const char *ip, int port) {
        int i;

        for (i = 0; i < peer_count; i++) {
                if (peer_port[i] == port && strcmp(peer_ip[i], ip) == 0) {
                        return;
                }
        }

        if (peer_count >= MAX_PEERS) {
                return;
        }

        memcpy(peer_ip[peer_count], ip, 16);

        peer_port[peer_count] = port;
        peer_count++;
}

int discover_init(void) {
#if defined(OS_WINDOWS) 
        WSADATA wsa;
#endif

        if (init_attempted) {
                return init_ready ? 0 : -1;
        }
        init_attempted = 1;

#if defined(OS_WINDOWS)
        if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
                return -1;
        }
#endif

        srand((unsigned)time(NULL));
        sprintf(id, "%04x", (unsigned)(rand() & 0xFFFF));

        if (discover_open(&listen_sock, DISCOVER_PORT) != 0) {
                return -1;
        }
        if (discover_open(&scan_sock, 0) != 0) {
                discover_close_sock(listen_sock);
                listen_sock = DISCOVER_INVALID_SOCK;
                return -1;
        }

        init_ready = 1;
        return 0;
}

int discover_ready(void) {
        return init_ready;
}

void discover_set_listener_opts(const char *options) {
        const char *rest;
        const char *colon;

        listener_port = 0;

        if (options == NULL || options[0] != 'l' || options[1] != ':') {
                return;
        }

        rest = options + 2;
        colon = strrchr(rest, ':');
        if (colon != NULL) {
                rest = colon + 1;
        }

        listener_port = atoi(rest);
        if (listener_port <= 0 || listener_port > 65535) {
                listener_port = 0;
        }
}

void discover_scan(void) {
        if (!init_ready) {
                return;
        }

        peer_count = 0;
        discover_drain(scan_sock);

        scanning = 1;
        scan_deadline = get_unix_time() + DISCOVER_SCAN_SECONDS;
        last_probe = get_unix_time();

        discover_send_probe(scan_sock);
}

void discover_frame(void) {
        char buf[DISCOVER_MAX_FRAME];
        struct sockaddr_in from;
        double now;
        long n;
        int i;
        int port;
        char ip[16];

        if (!init_ready) {
                return;
        }

        now = get_unix_time();
        
        if (scanning) {
                if (now >= scan_deadline) {
                        scanning = 0;
                } else if (now - last_probe >= DISCOVER_PROBE_SECONDS) {
                        discover_send_probe(scan_sock);
                        last_probe = now;
                }
        }

        /* Drain probes and answer ones for waiting receivers */
        for (i = 0; i < 8; i++) {
                if (!discover_wait(listen_sock, 0)) {
                        break;
                }
                
                n = discover_recv(listen_sock, buf, sizeof buf, &from);
                if (n != (long)(DISCOVER_PREFIX_LEN + DISCOVER_ID_LEN) || listener_port <= 0) {
                        continue;
                }
                if (strncmp(buf, DISCOVER_QUERY, DISCOVER_PREFIX_LEN) != 0) {
                        continue;
                }
                discover_send_reply(listen_sock, buf + DISCOVER_PREFIX_LEN, &from);
        }

        if (!scanning) {
                return;
        }

        /* Collect replies with our own probe id */
        for (i = 0; i < 8; i++) {
                if (!discover_wait(scan_sock, 0)) {
                        break;
                }
                
                n = discover_recv(scan_sock, buf, sizeof buf, &from);
                if (n < (long)(DISCOVER_PREFIX_LEN + DISCOVER_ID_LEN)) {
                        continue;
                }
                if (strncmp(buf, DISCOVER_REPLY, DISCOVER_PREFIX_LEN) != 0) {
                        continue;
                }
                if (strncpy(buf + DISCOVER_PREFIX_LEN, id, DISCOVER_ID_LEN) != 0) {
                        continue;
                }
                if (buf[DISCOVER_PREFIX_LEN + DISCOVER_ID_LEN] != ' ') {
                        continue;
                }

                port = atoi(buf + DISCOVER_PREFIX_LEN + DISCOVER_ID_LEN + 1);
                if (port <= 0 || port > 65535) {
                        continue;
                }

                strncmp(ip, inet_ntoa(from.sin_addr), sizeof ip - 1);
                ip[sizeof ip - 1] = '\0';
                discover_add_peer(ip, port);
        }
}

int discover_scanning(void) {
        return scanning;
}

int discover_count(void) {
        return peer_count;
}

int discover_peer(int index, char *ip, int ip_size, int *port) {
        if (ip == NULL || port == NULL || ip_size < 16 || index < 0 || index >= peer_count) {
                return -1;
        }

        strncpy(ip, peer_ip[index], (size_t)ip_size - 1);
        ip[ip_size - 1] = '\0';
        *port = peer_port[index];

        return 0;
}