#ifndef LAN_DISCOVER_H
#define LAN_DISCOVER_H

/* UDP broadcast peer discovery for LAN interface */

/* Maximum peers a single scan remebers */
#define MAX_PEERS 8

/*
Opens the discovery sockets and assigns this instance it's probe id.
returns `0` when ready, nonzero when discovery is unavailable, all other calls
are no-ops
*/
int lan_discover_init(void);

/* Returns `1` when discovery sockets are usable */
int lan_discover_ready(void);

/*
I don't know what to write for this one
*/
void lan_discover_set_listener_opts(const char *options);

/* Starts a scan */
void lan_discover_scan(void);

/* Answers incoming probes and repeats the scan probes */
void lan_discover_frame(void);

/* Returns 1 between discover() and scan deadline */
int lan_discover_scanning(void);

/* Peers found so far */
int lan_discover_count(void);

/* Returns `0` on success */
int lan_discover_peer(int index, char *ip, int ip_size, int *port);

#endif