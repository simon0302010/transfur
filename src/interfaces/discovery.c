#include "discovery.h"

#include <stdio.h>
#include <string.h>

#include "interfaces.h"
#include "lan/discover.h"

#if defined(OS_LINUX)
#include "bluetooth/bt_discover.h"
#endif

void discovery_init(void) {
        discover_init();
#if defined(OS_LINUX)
        bt_discover_init();
#endif
}

int discovery_supported(int interface_id) {
        if (interface_id == if_lan) {
                return discover_ready() ? 1 : 0;
        }
#if defined(OS_LINUX)
        if (interface_id == if_bluetooth) {
                return bt_discover_ready();
        }
#endif
        return DISCOVERY_UNSUPPORTED;
}

void discovery_scan(int interface_id) {
        if (interface_id == if_lan) {
                discover_scan();
                return;
        }
#if defined(OS_LINUX)
        if (interface_id == if_bluetooth) {
                bt_discover_scan();
        }
#endif
}

int discovery_scanning(int interface_id) {
        if (interface_id == if_lan) {
                return discover_scanning();
        }
#if defined(OS_LINUX)
        if (interface_id == if_bluetooth) {
                return bt_discover_scanning();
        }
#endif
        return 0;
}

int discovery_count(int interface_id) {
        if (interface_id == if_lan) {
                return discover_count();
        }
#if defined(OS_LINUX)
        if (interface_id == if_bluetooth) {
                return bt_discover_count();
        }
#endif
        return 0;
}

int discovery_peer(int interface_id, int index, char *out, int out_size) {
        char ip[16];
        int port;

        if (out == NULL || out_size < 1) {
                return -1;
        }

        if (interface_id == if_lan) {
                if (discover_peer(index, ip, sizeof ip, &port) != 0) {
                        return -1;
                }
                if ((int)strlen(ip) + 7 > out_size) {
                        return -1;
                }
                sprintf(out, "%s:%d", ip, port);
                return 0;
        }
#if defined(OS_LINUX)
        if (interface_id == if_bluetooth) {
                return bt_discover_peer(index, out, out_size);
        }
#endif
        return -1;
}

void discovery_set_listener_opts(int interface_id, const char *options) {
        if (options == NULL) {
                options = "";
        }

        if (interface_id == if_lan) {
                discover_set_listener_opts(options);
                return;
        }
#if defined(OS_LINUX) 
        if (interface_id == if_bluetooth) {
                bt_discover_set_listener_opts(options);
        }
#endif
}