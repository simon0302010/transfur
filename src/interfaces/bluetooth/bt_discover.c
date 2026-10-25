#include "bt_discover.h"

#if (defined(OS_LINUX) && defined(HAS_BLUETOOTH)) ||                           \
    defined(OS_WINDOWS) /* Supported systems */

#if defined(OS_LINUX)
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define inline __inline__

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#undef inline
#elif defined(OS_WINDOWS)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bluetoothapis.h>
#include <winsock2.h>
#include <ws2bth.h>

#endif

/* Service UUID for scanning/advertising */
static const uint8_t bt_service_uuid[16] = {
    0x74, 0x72, 0x61, 0x6e, 0x73, 0x66, 0x75, 0x72, /* "transfur" */
    0x2d, 0x64, 0x69, 0x73, 0x63, 0x6f, 0x76, 0x31  /* "-discov1" */
};

#define BT_DISCOVER_MAX_DEVICES 16
#define BT_DISCOVER_INQUIRY_SECONDS 6
#define BT_DISCOVER_CHANNEL_MIN 1
#define BT_DISCOVER_CHANNEL_MAX 30

static int init_done = 0;
static int init_ok = 0;

static int listener_channel = 0;

#if defined(OS_LINUX)
static sdp_session_t *adv_session = NULL;
static sdp_record_t *adv_record = NULL;

static pthread_mutex_t peers_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_t scan_thread;
static volatile int scan_running = 0;
static volatile int scanning = 0;

#elif defined(OS_WINDOWS)
static CRITICAL_SECTION peers_lock;
static int peers_lock_init = 0;
static HANDLE scan_thread = NULL;
static volatile int scan_running = 0;
static volatile int scanning = 0;
#endif

static char peer_addr[BT_DISCOVER_MAX_PEERS][18];
static unsigned int peer_channel[BT_DISCOVER_MAX_PEERS];
static int peer_count = 0;

static int bt_parse_listener_channel(const char *options);
static int bt_advertise_start(unsigned int channel);
static void bt_advertise_stop(void);
#if defined(OS_LINUX)
static void *bt_scan_worker(void *arg);
#elif defined(OS_WINDOWS)
static DWORD WINAPI bt_scan_worker(LPVOID arg);
#endif
static void bt_scan_once(void);
#if defined(OS_LINUX)
static int bt_browse_channel(const bdaddr_t *remote, unsigned int *out);
#elif defined(OS_WINDOWS)
static int bt_browse_channel(const BLUETOOTH_ADDRESS *remote,
                             unsigned int *out);
static void bt_addr_from_str(const BLUETOOTH_ADDRESS *ba, char out[18]);
#endif
static void bt_set_peers(char addrs[][18], unsigned int *channels, int count);

int bt_discover_init(void) {
#if defined(OS_LINUX)
        int dev_id;
#elif defined(OS_WINDOWS)
        WSADATA wsa;

        if (init_done) {
                return init_ok;
        }
        init_done = 1;
#endif

#if defined(OS_LINUX)
        dev_id = hci_get_route(NULL);
        if (dev_id < 0) {
                init_ok = 0;
                return 0;
        }
#elif defined(OS_WINDOWS)
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
                init_ok = 0;
                return 0;
        }

        if (!peers_lock_init) {
                InitializeCriticalSection(&peers_lock);
                peers_lock_init = 1;
        }
#endif

        init_ok = 1;
        return 1;
}

int bt_discover_ready(void) {
        return init_ok; /* This is just bullshit at this point */
}

static int bt_parse_listener_channel(const char *options) {
        const char *rest;
        const char *colon;
        unsigned long v;

        if (options == NULL || options[0] != 'l' || options[1] != ':') {
                return 0;
        }

        rest = options + 2;
        colon = strrchr(rest, ':');
        if (colon != NULL) {
                rest = colon + 1;
        }

        if (*rest == '\0') {
                return 0;
        }

        v = 0;
        while (*rest >= '0' && *rest <= 9) {
                v = v * 10 + (unsigned long)(*rest - '0');
                if (v > (unsigned long)BT_DISCOVER_CHANNEL_MAX) {
                        return 0;
                }
                rest++;
        }

        if (*rest != '\0' || v < (unsigned long)BT_DISCOVER_CHANNEL_MIN) {
                return 0;
        }

        return (int)v;
}

void bt_discover_set_listener_opts(const char *options) {
        int channel;

        if (!init_ok) {
                return;
        }

        channel = bt_parse_listener_channel(options);

        if (channel == 0) {
                if (listener_channel != 0) {
                        bt_advertise_stop();
                        listener_channel = 0;
                }
                return;
        }

        if (channel == listener_channel) {
                return;
        }

        if (listener_channel != 0) {
                bt_advertise_stop();
                listener_channel = 0;
        }

        if (bt_advertise_start((unsigned int)channel) == 0) {
                listener_channel = channel;
        }
}

#if defined(OS_LINUX)
static void *bt_scan_worker(void *arg) {
        (void)arg;

        bt_scan_once();

        scanning = 0;
        scan_running = 0;

        return NULL; /* void* must return something */
}
#elif defined(OS_WINDOWS)
static DWORD WINAPI bt_scan_worker(LPVOID arg) {
        (void)arg;

        bt_scan_once();

        scanning = 0;
        scan_running = 0;
        if (scan_thread != NULL) {
                scan_thread = NULL;
        }
        return 0;
}
#endif

void bt_discover_scan(void) {
        if (!init_ok || scan_running) {
                return;
        }
#if defined(OS_LINUX)
        pthread_mutex_lock(&peers_lock);
        peer_count = 0;
        pthread_mutex_unlock(&peers_lock);
#elif defined(OS_WINDOWS)
        EnterCriticalSection(&peers_lock);
        peer_count = 0;
        LeaveCriticalSection(&peers_lock);
#endif

        scanning = 1;
        scan_running = 1;
#if defined(OS_LINUX)
        if (pthread_create(&scan_thread, NULL, bt_scan_worker, NULL) != 0) {
                scanning = 0;
                scan_running = 0;
                return;
        }
        pthread_detach(scan_thread);
#elif defined(OS_WINDOWS)
        scan_thread = CreateThread(NULL, 0, bt_scan_worker, NULL, 0, NULL);
        if (scan_thread == NULL) {
                scanning = 0;
                scan_running = 0;
                return;
        }
        CloseHandle(scan_thread);
        scan_thread = NULL;
#endif
}

void bt_discover_frame(void) { /* mmmmpppfghh */ }

int bt_discover_scanning(void) { return scanning; }

int bt_discover_count(void) {
        int n;

#if defined(OS_LINUX)
        pthread_mutex_lock(&peers_lock);
        n = peer_count;
        pthread_mutex_unlock(&peers_lock);
#elif defined(OS_WINDOWS)
        EnterCriticalSection(&peers_lock);
        n = peer_count;
        LeaveCriticalSection(&peers_lock);
#endif

        return n;
}

int bt_discover_peer(int index, char *out, int out_size) {
        char addr[18];
        unsigned int channel;
        int n;

        if (out == NULL || out_size < 1) {
                return -1;
        }

#if defined(OS_LINUX)
        pthread_mutex_lock(&peers_lock);
        n = peer_count;
        if (index < 0 || index >= n) {
                pthread_mutex_unlock(&peers_lock);
                return -1;
        }
        memcpy(addr, peer_addr[index], 18);
        channel = peer_channel[index];
        pthread_mutex_unlock(&peers_lock);
#elif defined(OS_WINDOWS)
        EnterCriticalSection(&peers_lock);
        n = peer_count;
        if (index < 0 || index >= n) {
                LeaveCriticalSection(&peers_lock);
                return -1;
        }
        memcpy(addr, peer_addr[index], 18);
        channel = peer_channel[index];
        LeaveCriticalSection(&peers_lock);
#endif

        if (out_size < 22) {
                return -1;
        }

        sprintf(out, "%s:%u", addr, channel);
        return 0;
}

static void bt_set_peers(char addrs[][18], unsigned int *channels, int count) {
        int i;

#if defined(OS_LINUX)
        pthread_mutex_lock(&peers_lock);
        peer_count = count;
        for (i = 0; i < count; i++) {
                memcpy(peer_addr[i], addrs[i], 18);
                peer_channel[i] = channels[i];
        }
        pthread_mutex_unlock(&peers_lock);
#elif defined(OS_WINDOWS)
        EnterCriticalSection(&peers_lock);
        peer_count = count;
        for (i = 0; i < count; i++) {
                memcpy(peer_addr[i], addrs[i], 18);
                peer_channel[i] = channels[i];
        }
        LeaveCriticalSection(&peers_lock);
#endif
}

static void bt_scan_once(void) {
#if defined(OS_LINUX)
        inquiry_info *devices = NULL;
        char found_addr[BT_DISCOVER_MAX_PEERS][18];
        unsigned int found_channel[BT_DISCOVER_MAX_PEERS];
        int found = 0;
        int dev_id;
        int sock;
        int num;
        int i;

        dev_id = hci_get_route(NULL);
        if (dev_id < 0) {
                return;
        }

        sock = hci_open_dev(dev_id);
        if (sock < 0) {
                return;
        }

        /* hydrochloric acid inquiry 🤣🤣🤣🤣🤣🤣🤣 */
        num = hci_inquiry(dev_id, BT_DISCOVER_INQUIRY_SECONDS,
                          BT_DISCOVER_MAX_DEVICES, NULL, &devices,
                          IREQ_CACHE_FLUSH);
        if (num < 0) {
                hci_close_dev(sock);
                return;
        }

        for (i = 0; i < num && found < BT_DISCOVER_MAX_PEERS; i++) {
                unsigned int channel;
                bdaddr_t *bd;

                bd = &devices[i].bdaddr;
                if (bt_browse_channel(bd, &channel) != 0) {
                        continue;
                }

                ba2str(bd, found_addr[found]);
                found_channel[found] = channel;
                found++;
        }

        if (devices != NULL) {
                free(devices);
        }
        hci_close_dev(sock);

        bt_set_peers(found_addr, found_channel, found);
#elif defined(OS_WINDOWS)
        BLUETOOTH_DEVICE_SEARCH_PARAMS search_params;
        BLUETOOTH_DEVICE_INFO device_info;
        HBLUETOOTH_DEVICE_FIND find_handle;
        char found_addr[BT_DISCOVER_MAX_PEERS][18];
        unsigned int found_channel[BT_DISCOVER_MAX_PEERS];
        int found = 0;

        if (!init_ok) {
                return;
        }

        memset(&search_params, 0, sizeof(search_params));
        search_params.dwSize = sizeof(search_params);
        search_params.fReturnAuthenticated = TRUE;
        search_params.fReturnRemembered = TRUE;
        search_params.fReturnUnknown = TRUE;
        search_params.fReturnConnected = TRUE;
        search_params.fIssueInquiry = TRUE;
        search_params.cTimeoutMultiplier = BT_DISCOVER_INQUIRY_SECONDS;
        search_params.hRadio = NULL;

        memset(&device_info, 0, sizeof(device_info));
        device_info.dwSize = sizeof(device_info);

        find_handle = BluetoothFindFirstDevice(&search_params, &device_info);
        if (find_handle == NULL) {
                return;
        }

        do {
                unsigned int channel = 0;
                char addr[18];

                if (found >= BT_DISCOVER_MAX_PEERS) {
                        break;
                }

                if (bt_browse_channel(&device_info.Address, &channel) != 0) {
                        continue;
                }

                bt_addr_from_str(&device_info.Address, addr);
                memcpy(found_addr[found], addr, 18);
                found++;
        } while (BluetoothFindNextDevice(find_handle, &device_info));

        BluetoothFindDeviceClose(find_handle);

        bt_set_peers(found_addr, found_channel, found);
#endif
}

#if defined(OS_LINUX)
static int bt_browse_channel(const bdaddr_t *remote, unsigned int *out) {
        sdp_session_t *sess;
        uuid_t svc_uuid;
        sdp_list_t *search_list = NULL;
        sdp_list_t *attrid_list = NULL;
        sdp_list_t *response_list = NULL;
        sdp_list_t *r;
        uint32_t range = 0x0000ffff;
        int found = -1;

        sdp_uuid128_create(&svc_uuid, (void *)bt_service_uuid);

        search_list = sdp_list_append(NULL, &svc_uuid);
        attrid_list = sdp_list_append(NULL, &range);

        sess = sdp_connect(BDADDR_ANY, remote, SDP_RETRY_IF_BUSY);
        if (sess == NULL) {
                sdp_list_free(search_list, NULL);
                sdp_list_free(attrid_list, NULL);
                return -1;
        }

        if (sdp_service_search_attr_req(sess, search_list, SDP_ATTR_REQ_RANGE,
                                        attrid_list, &response_list) == 0) {
                for (r = response_list; r != NULL; r = r->next) {
                        sdp_record_t *rec;
                        sdp_list_t *protos = NULL;
                        int port;

                        rec = (sdp_record_t *)r->data;
                        if (sdp_get_access_protos(rec, &protos) == 0) {
                                port = sdp_get_proto_port(protos, RFCOMM_UUID);
                                if (port > 0) {
                                        found = 0;
                                        *out = (unsigned int)port;
                                }
                                sdp_list_free(protos, NULL);
                        }
                        sdp_record_free(rec);
                        if (found == 0) {
                                break;
                        }
                }
        }

        sdp_list_free(response_list, NULL);
        sdp_list_free(search_list, NULL);
        sdp_list_free(attrid_list, NULL);
        sdp_close(sess);

        return found;
}
#elif defined(OS_WINDOWS)
static void bt_addr_from_str(const BLUETOOTH_ADDRESS *ba, char out[18]) {
        unsigned long long val = ba->ullLong;
        unsigned char b[6];
        int i;

        for (i = 0; i < 6; i++) {
                b[5 - i] = (unsigned char)(val & 0xFF);
                val >>= 8;
        }

        sprintf(out, "%02X:%02X:%02X:%02X:%02X:%02X", b[0], b[1], b[2], b[3],
                b[4], b[5]);
}

static int bt_browse_channel(const BLUETOOTH_ADDRESS *remote,
                             unsigned int *out) {
        WSAQUERYSETW query_set;
        HANDLE lookup_handle = NULL;
        GUID service_guid;
        int found = -1;
        unsigned int j;

        if (remote == NULL || out == NULL) {
                return -1;
        }

        memcpy(&service_guid, bt_service_uuid, sizeof(GUID));

        memset(&query_set, 0, sizeof(query_set));
        query_set.dwSize = sizeof(query_set);
        query_set.dwNameSpace = NS_BTH;
        query_set.lpServiceClassId = &service_guid;

        if (WSALookupServiceBeginW(&query_set, LUP_FLUSHCACHE | LUP_RETURN_ADDR,
                                   &lookup_handle) != 0) {
                return -1;
        }

        while (1) {
                DWORD size = size(WSAQUERYSETW);
                BYTE buf[4096];
                LPWSAQUERYSETW p = (LPWSAQUERYSETW)buf;

                memset(buf, 0, sizeof(buf));
                p->dwSize = sizeof(WSAQUERYSETW);

                if (WSALookupServiceNextW(lookup_handle,
                                          LUP_FLUSHCACHE | LUP_RETURN_ADDR,
                                          &size, p) != 0) {
                        break;
                }

                if (p->dwNumberOfCsAddrs == 0 || p->lpscaBuffer == NULL) {
                        continue;
                }

                for (j = 0; j < p->dwNumberOfCsAddrs; j++) {
                        LPSOCKADDR addr =
                            p->lpcsaBuffer[j].RemoteAddr.lpSockaddr;
                        SOCKADDR_BTH *sa_bth;

                        if (addr == NULL || addr->sa_family != AF_BTH) {
                                continue;
                        }

                        sa_bth = (SOCKADDR_BTH *)addr;

                        if (sa_bth->btAddr != remote->ullLong) {
                                continue;
                        }

                        if (sa_bth->port < BT_DISCOVER_CHANNEL_MIN ||
                            sa_bth->port > BT_DISCOVER_CHANNEL_MAX) {
                                continue;
                        }

                        *out = (unsigned int)sa_bth->port;
                        found = 0;
                        break;
                }

                if (found == 0) {
                        break;
                }
        }

        WSALookupServiceEnd(lookup_handle);
        return found;
}
#endif

static int bt_advertise_start(unsigned int channel) {
#if defined(OS_LINUX)
        uuid_t root_uuid;
        sdp_list_t *root_list = NULL;
        uuid_t l2cap_uuid;
        uuid_t rfcomm_uuid;
        uuid_t svc_uuid;
        sdp_list_t *l2cap_list = NULL;
        sdp_list_t *rfcomm_list = NULL;
        sdp_list_t *proto_list = NULL;
        sdp_list_t *access_proto_list = NULL;
        sdp_list_t *svc_class_list = NULL;
        sdp_data_t *channel_data = NULL;
        sdp_record_t *record = NULL;
        unsigned char ch = (unsigned char)channel;
        int err = 0;

        record = sdp_record_alloc();
        if (record == NULL) {
                return -1;
        }

        sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
        root_list = sdp_list_append(NULL, &root_uuid);
        sdp_set_browse_groups(record, root_list);

        sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
        l2cap_list = sdp_list_append(NULL, &l2cap_uuid);
        proto_list = sdp_list_append(NULL, l2cap_list);

        sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
        rfcomm_list = sdp_list_append(NULL, &rfcomm_uuid);
        channel_data = sdp_data_alloc(SDP_UINT8, &ch);
        rfcomm_list = sdp_list_append(rfcomm_list, channel_data);
        proto_list = sdp_list_append(proto_list, rfcomm_list);

        access_proto_list = sdp_list_append(NULL, proto_list);
        sdp_set_access_protos(record, access_proto_list);

        sdp_uuid128_create(&svc_uuid, (void *)bt_service_uuid);
        svc_class_list = sdp_list_append(NULL, &svc_uuid);
        sdp_set_service_classes(record, svc_class_list);

        sdp_set_info_attr(record, "Transfur", NULL, NULL);

        adv_session = sdp_connect(BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY);
        if (adv_session == NULL) {
                err = -1;
        } else {
                err = sdp_record_register(adv_session, record, 0);
                if (err != 0) {
                        sdp_close(adv_session);
                        adv_session = NULL;
                }
        }

        sdp_list_free(root_list, NULL);
        sdp_list_free(access_proto_list, NULL);
        sdp_list_free(svc_class_list, NULL);
        sdp_list_free(proto_list, NULL);
        sdp_list_free(l2cap_list, NULL);
        sdp_list_free(rfcomm_list, NULL);
        if (channel_data != NULL) {
                sdp_data_free(channel_data);
        }

        if (err != 0) {
                sdp_record_free(record);
                return -1;
        }

        adv_record = record;
#endif

        return 0;
}

static void bt_advertise_stop(void) {
#if defined(OS_LINUX)
        if (adv_session != NULL && adv_record != NULL) {
                (void)sdp_record_unregister(adv_session, adv_record);
        }
        if (adv_record != NULL) {
                sdp_record_free(adv_record);
                adv_record = NULL;
        }
        if (adv_session != NULL) {
                sdp_close(adv_session);
                adv_session = NULL;
        }
#elif defined(OS_WINDOWS)
        return;
#endif
}

#else /* Unsupported systems */

int bt_discover_init(void) { return 0; }

int bt_discover_ready(void) { return 0; }

void bt_discover_set_listener_opts(const char *options) { (void)options; }

void bt_discover_scan(void) { /* spooky */ }

void bt_discover_frame(void) { /* lonely :< */ }

int bt_discover_scanning(void) { return 0; }

int bt_discover_count(void) { return 0; }

int bt_discover_peer(int index, char *out, int out_size) {
        (void)index;
        (void)out;
        (void)out_size;
        return -1;
}

#endif
