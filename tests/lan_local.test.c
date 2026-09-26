#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/interfaces/lan/lan.h"

#define T_DATA 0

static void put_be32(unsigned char *p, unsigned long v) {
        p[0] = (unsigned char)(v >> 24);
        p[1] = (unsigned char)(v >> 16);
        p[2] = (unsigned char)(v >> 8);
        p[3] = (unsigned char)(v >> 0);
}

static unsigned long get_be32(const unsigned char *p) {
        return ((unsigned long)p[0] << 24) | ((unsigned long)p[1] << 16) |
        ((unsigned long)p[2] << 8) | (unsigned long)p[3];
}

static int listener(int port) {
        char slot[512];
        char opts[64];
        struct chunk got;
        int r;

        sprintf(opts, "l:%d", port);
        memset(slot, 0, sizeof slot);

        r = init_conn_lan(slot, opts);
        if (r != LAN_OK) {
                fprintf(stderr, "listener: init_conn_lan failed: %d\n", r);
                return 1;
        }
        printf("listener: accepted connection\n");

        memset(&got, 0, sizeof got);
        r = recv_chunk_lan(slot, &got);
        if (r != LAN_OK) {
                fprintf(stderr, "listener: recv_chunk_lan failed: %d\n", r);
                return 1;
        }

        printf("listener: type=%d len=%lu data=\"%.*s\"\n", got.type, get_be32(got.length), (int)get_be32(got.length), got.data);

        if (got.type != T_DATA){
                return 1;
        }
        if (get_be32(got.length) != 5UL){
                return 1;
        }
        if (memcmp(got.data, "hello", 5) != 0) {
                return 1;
        }

        printf("listener: payload matches\n");
        return 0;
}

static int connector(int port) {
        char slot[512];
        char opts[64];
        struct chunk out;
        int r;

        sprintf(opts, "c:127.0.0.1:%d", port);
        memset(slot, 0, sizeof slot);

        r = init_conn_lan(slot, opts);
        if (r != LAN_OK) {
                fprintf(stderr, "connector: init_conn_lan failed: %d (is 'listen' running on that port?)\n", r);
                return 1;
        }
        printf("connector: connected\n");

        memset(&out, 0, sizeof out);
        out.type = T_DATA;
        put_be32(out.length, 5UL);
        memcpy(out.data, "hello", 5);

        r = send_chunk_lan(slot, &out);
        if (r != LAN_OK) {
                fprintf(stderr, "connector: send_chunk_lan failed: %d\n", r);
                return 1;
        }       

        printf("connector: sent 5 bytes and it's working yay\n");
        return 0;
}

int main(int argc, char **argv) {
        int port = 6767;

        if (argc == 3) {
                port = atoi(argv[2]);
        } else if (argc != 2) {
                fprintf(stderr, "usage: %s listen|connect [port]\n", argv[0]);
                return 2;
        }

        if (strcmp(argv[1], "listen") == 0) {
                return listener(port);
        }
        if (strcmp(argv[1], "connect") == 0) {
                return connector(port);
        } 

        fprintf(stderr, "usage: %s listen|connect [port]\n", argv[0]);
        return 2;
}