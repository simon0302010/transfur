/*
pip.c – short for public ip
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef OS_WINDOWS
#include <windows.h>
#include <wininet.h>
#else /* POSIX */
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include "pip.h"

#define BUF_SIZE 512 /* Aus'm Ärmel geschüttelt */
#define URL "ifconfig.me/ip"
#define PORT 80

int get_public_ip(char *dest, size_t n) {
#ifdef OS_WINDOWS
        HINTERNET hInternet, hConnect;
        DWORD bytesRead;
        CHAR buffer[BUF_SIZE];

        hInternet =
            InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

        if (!hInternet) {
                fprintf(stderr, "InternetOpen failed: %ld\n", GetLastError());
                return 1;
        }

        hConnect =
            InternetOpenUrl(hInternet, URL, NULL, 0, INTERNET_FLAG_RELOAD, 0);

        if (!hConnect) {
                fprintf(stderr, "InternetOpenUrl failed: %ld\n",
                        GetLastError());
                InternetCloseHandle(hInternet);
                return 1;
        }

        if (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1,
                             &bytesRead) &&
            bytesRead > 0) {
                buffer[bytesRead] = '\0';
        }

        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        if (bytesRead > 0) {
                strncpy(dest, buffer, n);
                return 0;
        } else {
                return bytesRead;
        }
#else
        struct hostent *server;
        int sockfd;
        struct sockaddr_in addr;
        char request[128];
        char buf[BUF_SIZE];
        int resn, i;
        char *ip;
        char host[64];
        char path[64];

        {
                char url[] = URL;
                char *slash = strchr(url, '/');
                if (slash == NULL || slash == url)
                        return 1;
                strcpy(path, slash);
                slash[0] = '\0';
                strcpy(host, url);
                if (host[0] == '\0' || path[0] == '\0')
                        return 1;
        }

        server = gethostbyname(host);
        if (server == NULL) {
                fprintf(stderr, "failed to lookup %s\n", host);
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
                path, host);
        send(sockfd, request, strlen(request), 0);

        resn = recv(sockfd, buf, BUF_SIZE - 1, 0);
        close(sockfd);
        if (resn <= 0) {
                perror("recv");
                return 1;
        }
        buf[resn] = '\0';

        for (i = 0; buf[i] != '\0'; i++) {
                if (strncmp(&buf[i], "\r\n\r\n", 4) == 0) {
                        strncpy(dest, &buf[i + 4], n);
                        return 0;
                }
        }

        return 1; /* failed to find the 2 newlines */
#endif
}