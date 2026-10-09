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

#include "../../misc/bool.h"
#include "pip.h"

#define BUF_SIZE 512 /* Aus'm Ärmel geschüttelt */
#define URL "ifconfig.me/ip"
#define PORT 80
#define IP_STRING_LEN 32

tbool connection_failed = false;
char public_ip[IP_STRING_LEN];

const char *get_public_ip(void) {
        if (public_ip[0] != '\0') {
                return public_ip;
        } else if (connection_failed) {
                return NULL;
        } else {
#ifdef OS_WINDOWS
                HINTERNET hInternet, hConnect;
                DWORD bytesRead;
                CHAR buffer[BUF_SIZE];

                hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL,
                                         NULL, 0);

                if (!hInternet) {
                        fprintf(stderr, "InternetOpen failed: %ld\n",
                                GetLastError());
                        connection_failed = true;
                        return NULL;
                }

                hConnect = InternetOpenUrl(hInternet, URL, NULL, 0,
                                           INTERNET_FLAG_RELOAD, 0);

                if (!hConnect) {
                        fprintf(stderr, "InternetOpenUrl failed: %ld\n",
                                GetLastError());
                        InternetCloseHandle(hInternet);
                        connection_failed = true;
                        return NULL;
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
#elif defined(OS_MACOS) || defined(OS_LINUX)
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
                        if (slash == NULL || slash == url) {
                                connection_failed = true;
                                return NULL;
                        }
                        strcpy(path, slash);
                        slash[0] = '\0';
                        strcpy(host, url);
                        if (host[0] == '\0' || path[0] == '\0') {
                                connection_failed = true;
                                return NULL;
                        }
                }

                server = gethostbyname(host);
                if (server == NULL) {
                        fprintf(stderr, "failed to lookup %s\n", host);
                        connection_failed = true;
                        return NULL;
                }

                sockfd = socket(AF_INET, SOCK_STREAM, 0);
                if (sockfd < 0) {
                        perror("socket");
                        return NULL;
                }

                memset(&addr, 0, sizeof(addr));
                addr.sin_family = AF_INET;
                addr.sin_port = htons(PORT);
                memcpy(&addr.sin_addr.s_addr, server->h_addr_list[0],
                       server->h_length);

                if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) <
                    0) {
                        perror("connect");
                        close(sockfd);
                        connection_failed = true;
                        return NULL;
                }

                sprintf(
                    request,
                    "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
                    path, host);
                send(sockfd, request, strlen(request), 0);

                resn = recv(sockfd, buf, BUF_SIZE - 1, 0);
                close(sockfd);
                if (resn <= 0) {
                        perror("recv");
                        connection_failed = true;
                        return NULL;
                }
                buf[resn] = '\0';

                for (i = 0; buf[i] != '\0'; i++) {
                        if (strncmp(&buf[i], "\r\n\r\n", 4) == 0) {
                                strncpy(public_ip, &buf[i + 4],
                                        IP_STRING_LEN - 1);
                                return 0;
                        }
                }

                connection_failed = true;
                return NULL; /* failed to find the 2 newlines */
#else
                connection_failed = true;
                return NULL;
#endif
        }
}