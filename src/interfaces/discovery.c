#include "discovery.h"

#include <stdio.h>
#include <string.h>

#include "interfaces.h"

/* No need to guard the includes */
#include "bluetooth/bt_discover.h"
#include "lan/lan_discover.h"

void discovery_init(void) {
        lan_discover_init();
        bt_discover_init();
}

void discovery_frame(void) {
        lan_discover_frame();
        bt_discover_frame();
}

int discovery_supported(enum interface interface) {
        if (interface == if_lan && lan_discover_ready() == 1)
                return DISCOVERY_OK;
        else if (interface == if_bluetooth && bt_discover_ready() == 1)
                return DISCOVERY_OK;

        return DISCOVERY_ERR_UNSUPPORTED;
}

void discovery_scan(enum interface interface) {
        if (interface == if_lan) {
                lan_discover_scan();
        } else if (interface == if_bluetooth) {
                bt_discover_scan();
        }
}

int discovery_scanning(enum interface interface) {
        if (interface == if_lan) {
                return lan_discover_scanning();
        } else if (interface == if_bluetooth) {
                return bt_discover_scanning();
        }
        return 0;
}

int discovery_count(enum interface interface) {
        if (interface == if_lan) {
                return lan_discover_count();
        } else if (interface == if_bluetooth) {
                return bt_discover_count();
        }
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
                if (lan_discover_peer(index, ip, sizeof ip, &port) != 0) {
                        return -1;
                }
                if (strlen(ip) + 7 > out_size) {
                        return -1;
                }
                sprintf(out, "%s:%d", ip, port);
                return 0;
        } else if (interface == if_bluetooth) {
                return bt_discover_peer(index, out, out_size);
        }
        return -1;
}

void discovery_set_listener_opts(enum interface interface,
                                 const char *options) {
        if (options == NULL) {
                options = "";
        }

        if (interface == if_lan) {
                lan_discover_set_listener_opts(options);
                return;
        } else if (interface == if_bluetooth) {
                bt_discover_set_listener_opts(options);
        }
}