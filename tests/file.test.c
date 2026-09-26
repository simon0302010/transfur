#include <stdio.h>
#include <string.h>

#include "src/interfaces/file/file.h"

#define TEST_PATH "/tmp/testing.bin"
#define MESSAGE "hello my sweet pookies"

static void put_be32(unsigned char *dst, unsigned long v) {
        dst[0] = (unsigned char)(v >> 24);
        dst[1] = (unsigned char)(v >> 16);
        dst[2] = (unsigned char)(v >> 8);
        dst[3] = (unsigned char)(v >> 0);
}

static unsigned long get_be32(const unsigned char *src) {
        return ((unsigned long)src[0] << 24) | ((unsigned long)src[1] << 16) | ((unsigned long)src[2] << 8) | (unsigned long)src[3];     
}

int main(void) {
        char slot[512];
        struct chunk out;
        struct chunk in;
        unsigned long len;
        int r;
        int failed = 0;

        /* Clear up old temporary test file */
        remove(TEST_PATH);

        memset(slot, 0, sizeof slot);
        r = init_conn_file(slot, TEST_PATH);
        if (r != 0) {
                printf("init_conn_file returned %d\n", r);
                return 1;
        }

        memset(&out, 0, sizeof out);
        out.type = 0;
        put_be32(out.length, (unsigned long)strlen(MESSAGE));
        memcpy(out.data, MESSAGE, strlen(MESSAGE));

        r = send_chunk_file(slot, &out);
        if (r != 0) {
                printf("send_chunk_file returned %d\n", r);
                return 1;
        }
        printf("wrote: %s\n", MESSAGE);

        memset(&in, 0, sizeof in);
        r = recv_chunk_file(slot, &in);
        if (r != 0) {
                printf("recv_chunk_file returned %d\n", r);
                return 1;
        }

        len = get_be32(in.length);
        printf("read %lu bytes: %.*s\n", len, (int)len, in.data);
        
        if (len < (unsigned long)strlen(MESSAGE) || memcmp(in.data, MESSAGE, strlen(MESSAGE)) != 0) {
                printf("payload mismatch\n");
                failed = 1;
        }

        memset(&in, 0, sizeof in);
        r = recv_chunk_file(slot, &in);
        if (r != 0 || get_be32(in.length) != 0UL || in.type != 1) {
                printf("expected EOF (length 0, type 1)\n");
                failed = 1;
        }

        remove(TEST_PATH);

        if (failed) {
                printf("write/read test pass yay");
        } else {
                printf("oh fuck it didn't work what");
        }

        return failed;
}