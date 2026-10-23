#ifndef DISCOVERY_H
#define DISCOVERY_H

#include "interfaces.h"

#define DISCOVERY_OK 0
#define DISCOVERY_ERR_UNKNOWN 1
#define DISCOVERY_ERR_UNSUPPORTED 2

void discovery_init(void);
void discovery_frame(void);
int discovery_supported(enum interface interface);
void discovery_scan(enum interface interface);
int discovery_scanning(enum interface interface);
int discovery_count(enum interface interface);
int discovery_peer(enum interface interface, int index, char *out,
                   int out_size);
void discovery_set_listener_opts(enum interface interface, const char *options);

#endif