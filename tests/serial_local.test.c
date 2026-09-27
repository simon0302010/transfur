#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "src/interfaces/file/file.h"
#include "src/interfaces/serial/serial.h"

#define T_DATA 0 
#define T_DONE 1
#define TIMEOUT_SECONDS 30

/* Very interesting names */
#define IN "victim.txt"
#define OUT "test_subject.txt"

/* Default payload when no file is specified */
#define PAYLOAD "kaboom"
/* Reciever reply when transfer is complete*/
#define REPLY "transfered"

static void put_be32(unsigned char *p, unsigned long v) {
        p[0] = (unsigned char)(v >> 24);
        p[1] = (unsigned char)(v >> 16);
        p[2] = (unsigned char)(v >> 8);
        p[3] = (unsigned char)(v >> 0);
}

static unsigned long get_be32(const unsigned char *p) {
        return ((unsigned long)p[0] << 24) | ((unsigned long)p[1] << 16) | ((unsigned long)p[2] << 8) | (unsigned long)p[3];
}

/* No progress for too long */
static void on_alarm(int sig) {
        static const char msg[] = "timed out";
        ssize_t ignored;

        (void)sig;
        ignored = write(2, msg, sizeof msg - 1);
        (void)ignored;
        _exit(1);
}

int main(int argc, char **argv) {
        char sslot[512];
        char fslot[512];
        const char *path;
        struct chunk chunk;
        unsigned long total;
        unsigned long checked;
        unsigned long len;
        int r;
        int chunks;
        int is_send;

        if (argc < 3 || argc > 4 || (strcmp(argv[1], "send") != 0 && strcmp(argv[1], "recv") != 0)) {
                fprintf(stderr, "usage: %s send|rev <device>[:baud] [file]\n", argv[0]);
                return 2;
        }

        is_send = strcmp(argv[1], "send") == 0;
        path = argc == 4 ? argv[3] : (is_send ? IN : OUT);

        signal(SIGALRM, on_alarm);
        alarm(TIMEOUT_SECONDS);

        memset(sslot, 0, sizeof sslot);
        r = init_conn_serial(sslot, argv[2]);
        if (r != SERIAL_OK) {
                fprintf(stderr, "init_conn_serial failed\n");
                return 1;
        }

        if (is_send) {
                /* Read payload */
                memset(fslot, 0, sizeof fslot);
                r = init_conn_file(fslot, path);
                if (r != 0) {
                        fprintf(stderr, "init_conn_file failed\n");
                        return 1;
                }

                total = 0;
                chunks = 0;
                for (;;) {
                        memset(&chunk, 0, sizeof chunk);
                        r = recv_chunk_file(fslot, &chunk);
                        if (r != 0) {
                                fprintf(stderr, "reading %s failed (file missing?)\n", path);
                                return 1;
                        }
                        if (chunk.type == T_DONE) {
                                break;
                        }

                        len = get_be32(chunk.length);
                        r = send_chunk_serial(sslot, &chunk);
                        if (r != SERIAL_OK) {
                                fprintf(stderr, "send_chunk_serial failed\n");
                                return 1;
                        }
                        total += len;
                        chunks++;
                        alarm(TIMEOUT_SECONDS);
                }

                memset(&chunk, 0, sizeof chunk);
                chunk.type = T_DONE;
                r = send_chunk_serial(sslot, &chunk);
                if (r != SERIAL_OK) {
                        fprintf(stderr, "send_chunk_file (end marker) failed\n");
                        return 1;
                }
                printf("send: sent %lu bytes in %d chunks from %s\n", total, chunks, path);

                /* reciever finish reply after it has saved everything */
                memset(&chunk, 0, sizeof chunk);
                r = recv_chunk_serial(sslot, &chunk);
                if (r != SERIAL_OK) {
                        fprintf(stderr, "waiting for reply failed");
                        return 1;
                }
                if (chunk.type != T_DATA || get_be32(chunk.length) != (unsigned long)strlen(REPLY) || memcmp(chunk.data, REPLY, strlen(reply)) != 0){
                        fprintf(stderr, "send: unexpected reply (type=%d len=%lu)\n", chunk.type, get_be32(chunk.length));
                        return 1;
                }
                printf("send: got reply \"%s\"\n", REPLY);
                alarm(0);
                printf("works yay\n")
                return 0;
        }

        /* recieve */
        memset(fslot, 0, sizeof fslot);
        remove(path);
        r = init_conn_file(fslot, path);
        if (r != 0) {
                fprintf(stderr, "init_conn_file failed\n");
                return 1;
        }

        total = 0;
        chunks = 0;
        for (;;) {
                memset(&chunk, 0, sizeof chunk);
                r = recv_chunk_serial(sslot, &chunk);
                if (r != SERIAL_OK) {
                        fprintf(stderr, "recv_chunk_serial failed\n");
                }
                if (chunk.type == T_DONE) {
                        break;
                }
                if (chunk.type != T_DATA) {
                        fprintf(stderr, "recv: unexpected chunk type %d\n", chunk.type);
                        return 1;
                }

                len = get_be32(chunk.length);
                r = send_chunk_file(fslot, &chunk);
                if (r != 0) {
                        fprintf(stderr, "writing %s failed\n", path);
                        return 1;
                }
                total += len;
                chunks++;
                alarm(TIMEOUT_SECONDS);
        }

        printf("recv: saved %lu bytes in %d chunks to %s\n", total, chunks, path);

        /* unblock sender before local verification */
        memset(&chunk, 0, sizeof chunk);
        chunk.type = T_DATA;
        put_be32(chunk.length, (unsigned long)strlen(REPLY));
        memcpy(chunk.data, REPLY, strlen(REPLY));
        r = send_chunk_serial(sslot, &chunk);
        if (r != SERIAL_OK) {
                fprintf(stderr, "send_chunk_serial (reply) failed\n");
                return 1;
        }

        /* read file back and compare total recieved vs total saved */
        checked = 0;
        for (;;) {
                memset(&chunk, 0, sizeof chunk);
                r = recv_chunk_file(fslot, &chunk);
                if (r != 0) {
                        fprintf(stderr, "reading back %s failed\n", path);
                        return 1;
                }
                if (chunk.type == T_DONE) {
                        break;
                }
                checked += get_be32(chunk.length);
        }
        if (checked != total) {
                fprintf(stderr, "recv: read back %lu bytes but saved %lu\n", checked, total);
                return 1;
        }

        printf("recv: replied \"%s\"\n", REPLY);
        alarm(0);
        printf("works yay\n");
        return 0;
}
