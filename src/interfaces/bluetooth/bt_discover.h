#ifndef BT_DISCOVER_H
#define BT_DISCOVER_H

/* Bluetooth SDP discovery */

#define BT_DISCOVER_MAX_PEERS 8

/*
Initializes the Bluetooth discovery backend

Returns 1 when discovery is usable, 0 otherwise
*/
int bt_discover_init(void);

/* 
Returns 1 when the Bluetooth discovery backend is usable 
*/
int bt_discover_ready(void);

/* 
Advertises a listening receiver over SDP so scanners can find it. 
*/
void bt_discover_set_listener_opts(const char *options);

/*
Starts an inquiry and a SDP browse scan for transfur receivers 
*/
void bt_discover_scan(void);

void bt_discover_frame(void);

/*
Returns 1 while a scan is in progress, 0 otherwise
*/
int bt_discover_scanning(void);

/*
Returns the number of peers found by the last scan
*/
int bt_discover_count(void);

/*
Writes the option string for peer index into out

Returns 0 on success -1 if index is out of range or out is too small
*/
int bt_discover_peer(int index, char *out, int out_size);

#endif