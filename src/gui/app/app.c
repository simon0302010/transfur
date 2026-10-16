#include "app.h"
#include "../../interfaces/interfaces.h"
#include "../../interfaces/lan/discover.h"
#include "../../interfaces/lan/lan.h"
#include "../../interfaces/lan/pip.h"
#include "../../interfaces/serial/serial.h"
#include "../core/widget.h"
#include "../core/widgets/button.h"
#include "../core/widgets/group.h"
#include "../core/widgets/loading_bar.h"
#include "../core/widgets/progress_bar.h"
#include "../core/widgets/split.h"
#include "../core/widgets/text.h"
#include "../core/widgets/text_input.h"

#include <stdio.h>
#include <string.h>

#if defined(OS_LINUX)
#include <pthread.h>

#elif defined(OS_WINDOWS)
#include <windows.h>

#ifdef interface
#undef interface
#endif

#elif defined(OS_MACOS)
#include <pthread.h>

#elif defined(OS_MSDOS)

#else
#error "Unsupported platform"

#endif

#define APP_WIDGET_COUNT 3

static struct widget widgets[APP_WIDGET_COUNT];
static struct widget recv_group[3];
static struct widget send_group[3];

static struct widget main_split[3];

/* receiving group */
static struct widget_button_options recv_button_opts;
static char recv_button_text[] = "No receiving interface";
static enum widget_button_styles recv_button_style =
    WIDGET_BUTTON_STYLE_FAILURE;

static struct widget_text_input_options recv_input_opts;
static char recv_input_text[] = "Input";

static struct widget_button_options recv_close_button_opts;
static char recv_close_button_text[] = "Close receiver";
static enum widget_button_styles recv_close_button_style =
    WIDGET_BUTTON_STYLE_DISABLED;

/* middle button */
static struct widget_button_options mid_button_opts;
static char mid_button_text[] = "Initialize Transaction";
static enum widget_button_styles mid_button_style =
    WIDGET_BUTTON_STYLE_DISABLED;

/* sending group */
static struct widget_button_options send_button_opts;
static char send_button_text[] = "No sending interface";
static enum widget_button_styles send_button_style =
    WIDGET_BUTTON_STYLE_FAILURE;

static struct widget_text_input_options send_input_opts;
static char send_input_text[] = "Input";

static struct widget_button_options send_close_button_opts;
static char send_close_button_text[] = "Close sender";
static enum widget_button_styles send_close_button_style =
    WIDGET_BUTTON_STYLE_DISABLED;

/* below the main split */
static char status[] = "Status: Idle";

static struct widget_progress_bar_options progress_bar_opts;
static char progress_bar_text[] = "Progress";
static float progress_bar_value = 0.0f;

/* transfur stuff */
static enum interface receiver = if_empty;
static enum interface sender = if_empty;

static char receiver_options[128] = "";
static char sender_options[128] = "";

/* Connection states, set to 1 after init_conn succeeds */
static volatile int receiver_connected = 0;
static volatile int sender_connected = 0;

/* State of the ip button for LAN options */
enum ip_btn_state { btn_local_ip, btn_public_ip };
static enum ip_btn_state ip_btn_state = btn_local_ip;

/*
Everything below is owned by the worker thread

worker_state is written by the worker and read by the UI.
The option/interface snapshots below are written by the UI while no worker is
running, then read by the worker.
*/
enum worker_state {
        wk_idle,
        wk_init_receiver,
        wk_init_sender,
        wk_wait_receiver,
        wk_wait_sender,
        wk_transfurring, /* Ha ha ha */
        wk_done,
        wk_error
};

/* worker_step values for wk_error */
#define STEP_INIT_RECEIVER 0
#define STEP_INIT_SENDER 1
#define STEP_RECEIVE 2
#define STEP_SEND 3

static volatile int worker_state = wk_idle;
static volatile int worker_if = (int)if_empty;
static volatile int worker_step = 0;
static volatile int worker_code = 0;
static volatile unsigned long worker_chunks = 0;

static char receiver_conn_opts[132] = "";
static char sender_conn_opts[132] = "";
static enum interface receiver_conn_if = if_empty;
static enum interface sender_conn_if = if_empty;

static volatile int receiver_inited = 0;
static volatile int sender_inited = 0;

static int receiver_conn = -1;
static int sender_conn = -1;

/* Build options string for init_conn */
static void build_conn_options(char *dst, enum interface if_type,
                               int is_receiver, const char *raw) {
        dst[0] = '\0';

        /* This is a bad way to handle that */
        if (if_type == if_lan) {
                strcpy(dst, is_receiver ? "l:" : "c:");
        }
        strcat(dst, raw);
}

static const char *error_text(int if_type, int code) {
        if (code == -1) {
                return "Failed to start the transfer thread";
        }

        return get_error_text(if_type, code);
}

static const char *step_text(int step) {
        switch (step) {
        case STEP_INIT_RECEIVER:
                return "Receiver initialized";
        case STEP_INIT_SENDER:
                return "Sender initialized";
        case STEP_RECEIVE:
                return "Receiving";
        case STEP_SEND:
                return "Sending";
        }

        return "Transfurring"; /* Pun */
}

/*
Builds the status line in buf
*/
static void status_text(char *buf) {
        strcpy(buf, "Status: "); /* Will append status text */

        switch (worker_state) {
        case wk_idle:
                sprintf(buf + strlen(buf), "Idle");
                break;
        case wk_init_receiver:
                sprintf(buf + strlen(buf), "Initializing receiver");
                break;
        case wk_init_sender:
                sprintf(buf + strlen(buf), "Initializing sender");
                break;
        case wk_wait_receiver:
                sprintf(buf + strlen(buf),
                        "Waiting for a peer to connect (receiver)");
                break;
        case wk_wait_sender:
                sprintf(buf + strlen(buf),
                        "Waiting for a peer to connect (sender)");
                break;
        case wk_transfurring:
                sprintf(buf + strlen(buf), "Transfurring, %lu chunks",
                        worker_chunks);
                break;
        case wk_done:
                sprintf(buf + strlen(buf), "Transfur complete (%lu chunk%s)",
                        worker_chunks, worker_chunks > 1 ? "s" : "");
                break;
        case wk_error:
                sprintf(buf + strlen(buf), "%s failed: %s",
                        step_text(worker_step),
                        error_text(worker_if, worker_code));
                break;
        }

        return;
}

#if defined(OS_WINDOWS)
static DWORD WINAPI transfer_worker(LPVOID arg)
#elif defined(OS_MSDOS)
static int transfer_worker(void *arg)
#else
static void *transfer_worker(void *arg)
#endif
{
        struct chunk chunk;
        int r;

        (void)arg;

        /* Both sides are initialized because init_conn can block but must not
         * freeze UI */
        if (!receiver_inited) {
                worker_state = (receiver_conn_if == if_lan) ? wk_wait_receiver
                                                            : wk_init_receiver;
                r = init_conn(&receiver_conn, receiver_conn_if,
                              receiver_conn_opts);

                if (r != 0) {
                        /* init_conn claims the slot before the interface runs,
                         * free it on failure */
                        close_conn(&receiver_conn);
                        receiver_conn = -1;
                        worker_if = (int)receiver_conn_if;
                        worker_step = STEP_INIT_RECEIVER;
                        worker_code = r;
                        worker_state = wk_error;
#if defined(OS_WINDOWS) || defined(OS_MSDOS)
                        return 0;
#else
                        return NULL;
#endif
                }
                receiver_inited = 1;
                receiver_connected = 1;
        }

        if (!sender_inited) {
                worker_state = (sender_conn_if == if_lan) ? wk_wait_sender
                                                          : wk_init_sender;
                r = init_conn(&sender_conn, sender_conn_if, sender_conn_opts);

                if (r != 0) {
                        /* Same as receiver side */
                        close_conn(&sender_conn);
                        sender_conn = -1;
                        worker_if = (int)sender_conn_if;
                        worker_step = STEP_INIT_SENDER;
                        worker_code = r;
                        worker_state = wk_error;
#if defined(OS_WINDOWS) || defined(OS_MSDOS)
                        return 0;
#else
                        return NULL;
#endif
                }
                sender_inited = 1;
                sender_connected = 1;
        }

        worker_chunks = 0;
        worker_state = wk_transfurring;

        /* Send chunks from the receiver into the sender until done */
        for (;;) {
                r = recv_chunk(&receiver_conn, &chunk);
                if (r != 0) {
                        worker_if = (int)receiver_conn_if;
                        worker_step = STEP_RECEIVE;
                        worker_code = r;
                        worker_state = wk_error;
                        break;
                }

                r = send_chunk(&sender_conn, &chunk);
                if (r != 0) {
                        worker_if = (int)sender_conn_if;
                        worker_step = STEP_SEND;
                        worker_code = r;
                        worker_state = wk_error;
                        break;
                }

                worker_chunks++;

                /* chunk.type 1 marks end of transfur */
                if (chunk.type == 1) {
                        worker_state = wk_done;
                        break;
                }
        }

#if defined(OS_WINDOWS) || defined(OS_MSDOS)
        return 0;
#else
        return NULL;
#endif
}

static int start_transfer_worker(void) {
#if defined(OS_WINDOWS)
        HANDLE thread;

        thread = CreateThread(NULL, 0, transfer_worker, NULL, 0, NULL);
        if (thread == NULL) {
                return -1;
        }
        CloseHandle(thread);
#elif defined(OS_MSDOS)
        transfer_worker(NULL);

        return 0;
#else
        pthread_t thread;

        if (pthread_create(&thread, NULL, transfer_worker, NULL) != 0) {
                return -1;
        }
        pthread_detach(thread);
#endif
        return 0;
}

static int worker_active(void) {
        return worker_state != wk_idle && worker_state != wk_done &&
               worker_state != wk_error;
}

static void cycle_receiver(void) {
        const enum interface *supported;
        int i;

        if (receiver_connected || worker_active())
                return;

        supported = get_supported_interfaces();

        i = 0;
        while (supported[i] != if_empty && supported[i] != receiver)
                i++;
        if (supported[i] == if_empty || supported[i + 1] == if_empty)
                receiver = supported[0];
        else
                receiver = supported[i + 1];

        recv_button_opts.title = (char *)get_receiver_text(receiver);
}

static void cycle_sender(void) {
        const enum interface *supported;
        int i;

        if (sender_connected || worker_active())
                return;

        supported = get_supported_interfaces();

        i = 0;
        while (supported[i] != if_empty && supported[i] != sender)
                i++;
        if (supported[i] == if_empty || supported[i + 1] == if_empty)
                sender = supported[0];
        else
                sender = supported[i + 1];

        send_button_opts.title = (char *)get_sender_text(receiver);
}

static void close_receiver(void) {
        if (worker_active() || !receiver_connected)
                return;
        close_conn(&receiver_conn);
        receiver_conn = -1;
        receiver_inited = 0;
        receiver_connected = 0;
        worker_state = wk_idle;
}

static void close_sender(void) {
        if (worker_active() || !sender_connected)
                return;
        close_conn(&sender_conn);
        sender_conn = -1;
        sender_inited = 0;
        sender_connected = 0;
        worker_state = wk_idle;
}

static void init_transaction(void) {
        int can_start = !worker_active() && receiver != if_empty &&
                        sender != if_empty &&
                        (receiver_connected || receiver_options[0] != '\0') &&
                        (sender_connected || sender_options[0] != '\0');

        if (!can_start)
                return;

        receiver_conn_if = receiver;
        sender_conn_if = sender;
        build_conn_options(receiver_conn_opts, receiver, 1, receiver_options);
        build_conn_options(sender_conn_opts, sender, 0, sender_options);

        worker_chunks = 0;
        worker_code = 0;
        worker_state = wk_init_receiver;
        if (start_transfer_worker() != 0) {
                worker_if = (int)receiver_conn_if;
                worker_step = STEP_INIT_RECEIVER;
                worker_code = -1;
                worker_state = wk_error;
        }
}

void app_init(struct ui_context *ctx) {
        recv_button_opts.title = (char *)get_receiver_text(receiver);
        recv_button_opts.style = &recv_button_style;
        recv_button_opts.callback = cycle_receiver;
        recv_input_opts.buffer = receiver_options;
        recv_input_opts.max_len = sizeof(receiver_options);
        recv_input_opts.cursor = 0;
        recv_input_opts.label = recv_input_text;
        recv_close_button_opts.title = recv_close_button_text;
        recv_close_button_opts.style = &recv_close_button_style;
        recv_close_button_opts.callback = close_receiver;

        recv_group[0] =
            widget_button(&recv_button_opts, sizeof(recv_button_opts));
        recv_group[1] =
            widget_text_input(&recv_input_opts, sizeof(recv_input_opts));
        recv_group[2] = widget_button(&recv_close_button_opts,
                                      sizeof(recv_close_button_opts));

        mid_button_opts.title = mid_button_text;
        mid_button_opts.style = &mid_button_style;
        mid_button_opts.callback = init_transaction;

        send_button_opts.title = (char *)get_sender_text(sender);
        send_button_opts.style = &send_button_style;
        send_button_opts.callback = cycle_sender;
        send_input_opts.buffer = sender_options;
        send_input_opts.max_len = sizeof(sender_options);
        send_input_opts.cursor = 0;
        send_input_opts.label = send_input_text;
        send_close_button_opts.title = send_close_button_text;
        send_close_button_opts.style = &send_close_button_style;
        send_close_button_opts.callback = close_sender;

        send_group[0] =
            widget_button(&send_button_opts, sizeof(send_button_opts));
        send_group[1] =
            widget_text_input(&send_input_opts, sizeof(send_input_opts));
        send_group[2] = widget_button(&send_close_button_opts,
                                      sizeof(send_close_button_opts));

        main_split[0] = widget_group(recv_group, 3);
        main_split[1] =
            widget_button(&mid_button_opts, sizeof(mid_button_opts));
        main_split[2] = widget_group(send_group, 3);

        widgets[0] = widget_split(main_split, 3);
        widgets[1] = widget_text(sizeof(status), status);

        progress_bar_opts.progress = &progress_bar_value;
        progress_bar_opts.title = progress_bar_text;

        widgets[2] =
            widget_progress_bar(&progress_bar_opts, sizeof(progress_bar_opts));

        ctx->widgets = widgets;
        ctx->count = APP_WIDGET_COUNT;
}

void app_update(double dt) {
        int can_start;

        discover_init();
        discover_set_listener_opts(
            (worker_state == wk_wait_receiver && receiver_conn_if == if_lan)
                ? receiver_conn_opts
                : "");
        discover_frame();

        status_text(status);

        can_start = !worker_active() && receiver != if_empty &&
                    sender != if_empty &&
                    (receiver_connected || receiver_options[0] != '\0') &&
                    (sender_connected || sender_options[0] != '\0');

        mid_button_style = can_start ? WIDGET_BUTTON_STYLE_PRIMARY
                                     : WIDGET_BUTTON_STYLE_DISABLED;

        recv_close_button_style = (!worker_active() && receiver_connected)
                                      ? WIDGET_BUTTON_STYLE_FAILURE
                                      : WIDGET_BUTTON_STYLE_DISABLED;

        send_close_button_style = (!worker_active() && sender_connected)
                                      ? WIDGET_BUTTON_STYLE_FAILURE
                                      : WIDGET_BUTTON_STYLE_DISABLED;

        recv_button_style = receiver_connected ? WIDGET_BUTTON_STYLE_SUCCESS
                            : (worker_state == wk_wait_receiver ||
                               worker_state == wk_init_receiver)
                                ? WIDGET_BUTTON_STYLE_PRIMARY
                                : WIDGET_BUTTON_STYLE_FAILURE;

        recv_button_style =
            sender_connected ? WIDGET_BUTTON_STYLE_SUCCESS
            : (worker_state == wk_wait_sender || worker_state == wk_init_sender)
                ? WIDGET_BUTTON_STYLE_PRIMARY
                : WIDGET_BUTTON_STYLE_FAILURE;

        if (worker_state == wk_done) {
                progress_bar_value = 1.0f;
        } else if (!worker_active()) {
                progress_bar_value = 0.0f;
        } else {
                /* make it look like its going towards 100% but keeps getting
                 * slower */
                /* because transfur is evil 😈 */
                float remaining = 1.0f - progress_bar_value;
                progress_bar_value += 0.1f * remaining;
        }
}