/*
pip.c – short for public ip
*/

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 512
#define HOST "ifconfig.me"
#define PATH "/ip"
#define PORT 80

int get_public_ip(char *dest, size_t n) {
        struct hostent *server;
        int sockfd;
        struct sockaddr_in addr;
        char request[128];
        char buf[BUF_SIZE];
        int n, i;
        char *ip;

        server = gethostbyname(HOST);
        if (server == NULL) {
                fprintf(stderr, "failed to lookup %s\n", HOST);
                return 1;
        }

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
                perror("socket");
                return 1;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        memcpy(&addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

        if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("connect");
                close(sockfd);
                return 1;
        }

        sprintf(request,
                "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
                PATH, HOST);
        send(sockfd, request, strlen(request), 0);

        n = recv(sockfd, buf, BUF_SIZE - 1, 0);
        if (n <= 0) {
                perror("recv");
                return 1;
        }
        buf[n] = '\0';

        for (i = 0; buf[i] != '\0'; i++) {
                if (strncmp(&buf[i], "\r\n\r\n", 4) == 0) {
                        strncpy(dest, &buf[i + 4], n);
                }
        }

        close(sockfd);
        return 0;
}