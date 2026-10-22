#ifndef DISCOVERY_H
#define DISCOVERY_H

#define DISCOVERY_UNSUPPORTED 0
#define DISCOVERY_SUPPORTED 1

void discovery_init(void);
void discovery_frame(void);
int discovery_supported(int interface_id);
void discovery_scan(int interface_id);
int discovery_scanning(int interface_id);
int discovery_count(int interface_id);
int discovery_peer(int interface_id, int index, char *out, int out_size);
void discovery_set_listener_opts(int interface_id, const char *options);

#endif