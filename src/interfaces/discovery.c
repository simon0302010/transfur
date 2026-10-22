#include "discovery.h"

#include <stdio.h>
#include <string.h>

#include "interfaces.h"
#include "lan/lan_discover.h"

#if defined(OS_LINUX)
#include "bluetooth/bt_discover.h"
#endif

void discovery_init(void) {
        discover_init();
#if defined(OS_LINUX)
        bt_discover_init();
#endif
}

int discovery_supported(enum interface interface) {
        if (interface == if_lan) {
                return discover_ready() ? 1 : 0;
        }
#if defined(OS_LINUX)
        if (interface == if_bluetooth) {
                return bt_discover_ready();
        }
#endif
        return DISCOVERY_UNSUPPORTED;
}

void discovery_scan(enum interface interface) {
        if (interface == if_lan) {
                discover_scan();
                return;
        }
#if defined(OS_LINUX)
        if (interface == if_bluetooth) {
                bt_discover_scan();
        }
#endif
}

int discovery_scanning(enum interface interface) {
        if (interface == if_lan) {
                return discover_scanning();
        }
#if defined(OS_LINUX)
        if (interface == if_bluetooth) {
                return bt_discover_scanning();
        }
#endif
        return 0;
}

int discovery_count(enum interface interface) {
        if (interface == if_lan) {
                return discover_count();
        }
#if defined(OS_LINUX)
        if (interface == if_bluetooth) {
                return bt_discover_count();
        }
#endif
        return 0;
}

int discovery_peer(enum interface interface, int index, char *out,
                   int out_size) {
        char ip[16];
        int port;

        if (out == NULL || out_size < 1) {
                return -1;
        }

        if (interface == if_lan) {
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
        if (interface == if_bluetooth) {
                return bt_discover_peer(index, out, out_size);
        }
#endif
        return -1;
}

void discovery_set_listener_opts(enum interface interface,
                                 const char *options) {
        if (options == NULL) {
                options = "";
        }

        if (interface == if_lan) {
                discover_set_listener_opts(options);
                return;
        }
#if defined(OS_LINUX)
        if (interface == if_bluetooth) {
                bt_discover_set_listener_opts(options);
        }
#endif
}