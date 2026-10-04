#ifndef DISCOVER_H
#define DISCOVER_H

/* UDP broadcast peer discovery for LAN interface */

/* Maximum peers a single scan remebers */
#define MAX_PEERS 8

/*
Opens the discovery sockets and assigns this instance it's probe id.
returns `0` when ready, nonzero when discovery is unavailable, all other calls are no-ops
*/
int discover_init(void);

/* Returns `1` when discovery sockets are usable */
int discover_ready(void);

/*
I don't know what to write for this one
*/
void discover_set_listener_opts(const char *options);

/* Starts a scan */
void discover_scan(void);

/* Answers incoming probes and repeats the scan probes */
void discover_frame(void);

/* Returns 1 between discover() and scan deadline */
int discover_scanning(void);

/* Peers found so far */
int discover_count(void);

/* Returns `0` on success */
int discover_peer(int index, char *ip, int ip_size, int *port);

#endif