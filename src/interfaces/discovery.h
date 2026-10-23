#ifndef DISCOVERY_H
#define DISCOVERY_H

#include "../misc/bool.h"
#include "interfaces.h"

/* No error */
#define DISCOVERY_OK 0
/* Unknown error. Avoid using. */
#define DISCOVERY_ERR_UNKNOWN 1
/* Interface doesn't support discovery */
#define DISCOVERY_ERR_UNSUPPORTED 2
/* The peer info output is too small or the index doesn't exist */
#define DISCOVERY_ERR_PEER 3

void discovery_init(void);
void discovery_frame(void);
int discovery_ready(enum interface interface);
void discovery_scan(enum interface interface);
/* Returns `true` if a scan is ongoing. `false` is returned if the interface
 * type is unsupported or no scan is running. */
tbool discovery_scanning(enum interface interface);
/* Returns the amount of discoverd devices. */
int discovery_count(enum interface interface);
int discovery_peer(enum interface interface, int index, char *out,
                   int out_size);
void discovery_set_listener_opts(enum interface interface, const char *options);

#endif