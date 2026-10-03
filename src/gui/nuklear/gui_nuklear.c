/*
Source file for Nuklear GUI code.
Needs to implement all functions defined in `gui.h`
*/

#include <stdio.h>
#include <string.h>

#include "../../interfaces/interfaces.h"
#include "../../interfaces/lan/lan.h"
#include "../../interfaces/serial/serial.h"
#include "../gui.h"

#include "nuklear.h"

#if defined(__linux__)
#include <pthread.h>
#include "linux/nuklear_linux.h"

#elif defined(_WIN32)
#include <windows.h>
#include "windows/nuklear_windows.h"

#ifdef interface
#undef interface 
#endif

#elif defined(__APPLE__)
#include <pthread.h>
#include "macos/nuklear_macos.h"

#else 
#error "Unsupported platform

static nk_size loading_bar_state = 0;

static enum interface receiver = if_empty;
static enum interface sender = if_empty;

static char receiver_options[128] = "";
static char sender_options[128] = "";

/* Connection states, set to 1 after init_conn succeeds */
static volatile int receiver_connected = 0;
static volatile int sender_connected = 0;

/*
Everything below is owned by the worker thread

worker_state is written by the worker and read by the UI.
The option/interface snapshots below are written by the UI while no worker is running, then read by the worker.
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
static void build_conn_options(char *dst, enum interface if_type, int is_receiver, const char *raw) {
        dst[0] = '\0';

        if (if_type == if_lan) {
                strcpy(dst, is_receiver ? "l:" : "c:");
        }
        strcat(dst, raw);
}

static const char *error_text(int if_type, int code) {
        if (code == -1) {
                return "could not start the transfer thread";
        }

        switch (if_type) {
        case if_empty:
                return "no interface selected"; 
        case if_file:
                switch (code) {
                case 1:
                        return "bad path or file IO error";
                case 2:
                        return "connection slots full (restart application)";
                default: 
                        return "unknown file error";
                }
        case if_lan:
                switch (code) {
                case LAN_ERR_ARG:
                        return "bad argument";
                case LAN_ERR_OPTIONS:
                        return "invalid options (use <port> or <ip>:<port>)";
                case LAN_ERR_STATE:
                        return "LAN slot already initialized";
                case LAN_ERR_SOCKET:
                        return "could not create a socket"; /* 😥 */
                case LAN_ERR_BIND:
                        return "could not bind to the address";
                case LAN_ERR_LISTEN:
                        return "could not listen on the address";
                case LAN_ERR_ACCEPT:
                        return "connection attempt failed";
                case LAN_ERR_CONNECT:
                        return "could not connect to the peer";
                case LAN_ERR_SEND:
                        return "send failed";
                case LAN_ERR_RECV:
                        return "receive failed";
                case LAN_ERR_FRAME:
                        return "received a malformed_frame";
                case LAN_ERR_PLATFORM:
                        return "LAN is not supported on this system yet"
                default: 
                        return "unknown LAN error";
                }
        case if_serial:
                switch (code) {
                case SERIAL_ERR_ARG:
                        return "bad argument";
                case SERIAL_ERR_OPTIONS:
                        return "invalid options (use <path>[:<baud>])";
                case SERIAL_ERR_STATE:
                        return "serial slot already initialized";
                case SERIAL_ERR_OPEN:
                        return "could not open the serial port";
                case SERIAL_ERR_CONFIG:
                        return "could not configure the serial port";
                case SERIAL_ERR_SEND:
                        return "send failed";
                case SERIAL_ERR_RECV:
                        return "receive failed";
                case SERIAL_ERR_CLOSED:
                        return "the peer closed the connection";
                case SERIAL_ERR_FRAME:
                        return "received a malformed frame";
                case SERIAL_ERR_PLATFORM:
                        return "serial is not supported on this system yet";
                }
        }

        return "unknown error";
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
static const char *status_text(char *buf) {
        switch (worker_state) {
        case wk_idle:
                return "Idle";
        case wk_init_receiver:
                return "Initializing receiver";
        case wk_init_sender:
                return "Initializing sender";
        case wk_wait_receiver:
                return "Waiting for a peer to connect (receiver)";
        case wk_wait_sender:
                return "Waiting for a peer to connect (sender)";
        case wk_transfurring:
                sprintf(buf, "Transfurring, %lu chunks", worker_chunks);
                return buf;
        case wk_done:
                sprintf(buf, "Transfur complete (%lu chunks)", worker_chunks);
                return buf;
        case wk_error:
                sprintf(buf, "%s failed: %s", step_text(worker_step), error_text(worker_if, worker_code));
                return buf;
        }
        
        return "";
}

#if defined(_WIN32)
static DWORD WINAPI transfer_worker(LPVOID arg)
#else
static void *transfer_worker(void *arg) 
#endif
{
        struct chunk chunk;
        int r;

        (void)arg;

        /* Both sides are initialized because init_conn can block but must not freeze UI */
        if (!receiver_inited) {
                worker_state = (receiver_conn_if == if_lan) ? wk_wait_receiver : wk_init_receiver;
                r = init_conn(&receiver_conn, receiver_conn_if, receiver_conn_opts);

                if (r != 0) {
                        worker_if = (int)receiver_conn_if;
                        worker_step = STEP_INIT_RECEIVER;
                        worker_code = r;
                        worker_state = wk_error;
#if defined(_WIN32)
                        return 0;
#else
                        return NULL;
#endif
                }
                receiver_inited = 1;
                receiver_connected = 1;
        }
        
        if(!sender_inited) {
                worker_state = (sender_conn_if == if_lan) ? wk_wait_sender : wk_init_sender;
                r = init_conn(&sender_conn, sender_conn_if, sender_conn_opts);

                if (r != 0) {
                        worker_if = (int)sender_conn_if;
                        worker_step = STEP_INIT_SENDER;
                        worker_code = r;
                        worker_state = wk_error;
#if defined(_WIN32)
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

#if defined(_WIN32)
        return 0;
#else
        return NULL;
#endif
}

static int start_transfer_worker(void) {
#if defined(_WIN32)
        HANDLE thread;
        
        thread = CreateThread(NULL, 0, transfer_worker, NULL, 0, NULL);
        if (thread == NULL) {
                return -1;
        }
        CloseHandle(thread);
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
        return worker_state != wk_idle && worker_state != wk_done && worker_state != wk_error;
}

static void nk_gui(const char *title, struct nk_context *ctx, int width, int height) {
        char status[192];
        int can_start;
        int bar_visible;

        status[0] = '\0';

        if (nk_begin(ctx, title, nk_rect(0, 0, width, height), 0)) {
                ctx->style.menu_button = ctx->style.button;
                
                nk_layout_row_dynamic(ctx, 20, 0);

                nk_layout_row_template_begin(ctx, 50);
                nk_layout_row_template_push_static(ctx, 180);
                nk_layout_row_template_push_dynamic(ctx);
                nk_layout_row_template_push_static(ctx, 180);
                nk_layout_row_template_push_dynamic(ctx);
                nk_layout_row_template_push_static(ctx, 180);
                nk_layout_row_template_end(ctx);

                /* Set color based on connection status */
                ctx->style.menu_button.normal = nk_style_item_color(
                        receiver_connectedd ? nk_rgba(0, 100, 0, 255)
                                            : nk_rgba(100, 0, 0, 255));
                ctx->style.menu_button.hover = nk_style_item_color(
                        receiver_connected ? nk_rgba(0, 80, 0, 255)
                                           : nk_rgba(80, 0, 0, 255));
                ctx->style.menu_button.active = nk_style_item_color(
                        receiver_connected ? nk_rgba(0, 60, 0, 255)
                                           : nk_rgba(60, 0, 0, 255));
                if (receiver_connected) {
                        nk_widget_disable_begin(ctx);
                }
                if (nk_menu_begin_label(ctx, get_receiver_text(receiver), NK_TEXT_CENTERED, nk_vec2(180, 120))) {
                        int i;
                        const enum interface *supported = get_supported_interfaces();

                        nk_layout_row_dynamic(ctx, 30, 1);

                        for (i = 0; supported[i] != if_empty; i++) {
                                if (nk_menu_item_label(ctx, get_receiver_text(supported[i]), NK_TEXT_LEFT)) {
                                        receiver = supported[i];
                                }
                        }
                        nk_menu_end(ctx);
                }
                if (reciever_connected) {
                        nk_widget_disable_end(ctx);
                }

                nk_spacer(ctx);

                /* A side that is already connected cannot be re-inited as there is no way to close a connection yet */
                can_start = !worker_action() && receiver != if_empty && sender != if_empty && (receiver_connected || receiver_options[0] != '\0') && (sender_connected || sender_options[0] != '\0');

                if (!can_start) {
                        nk_widget_disable_begin(ctx);
                }
                if (nk_button_label(ctx, "Initialize connection")) {
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

                if (!can_start) {
                        nk_widget_disable_end(ctx);
                }

                nk_spacer(ctx);

                /* Set color based on connection status */
                ctx->style.menu_button.normal = nk_style_item_color(
                        sender_connected ? nk_rgba(0, 100, 0, 255)
                                         : nk_rgba(100, 0, 0, 255));
                ctx->style.menu_button.hover = nk_style_item_color(
                        sender_connected ? nk_rgba(0, 80, 0, 255)
                                         : nk_rgba(80, 0, 0, 255));
                ctx->style.menu_button.active = nk_style_item_color(
                        sender_connected ? nk_rgba(0, 60, 0, 255)
                                         : nk_rgba(60, 0, 0, 255));
                
                if (sender_connected) {
                        nk_widget_disable_begin(ctx);
                }
                if (nk_menu_begin_label(ctx, get_sender_text(sender), NK_TEXT_CENTERED, nk_vec2(180, 120))) {
                        int i;
                        const enum interface *supported = get_supported_interfaces();

                        nk_layout_row_dynamic(ctx, 30, 1);

                        for (i = 0; supported[i] != if_empty; i++) {
                                if (nk_menu_item_label(ctx, get_sender_text(supported[i]), NK_TEXT_LEFT)) {
                                        sender = supported[i];
                                }
                        }
                        nk_menu_end(ctx);
                }
                if (sender_connected) {
                        nk_widget_disable_end(ctx);
                }

                /* Vertical spacer */
                nk_layout_row_dynamic(ctx, 10, 1);

                /* Row for option labels */
                nk_layout_row_template_begin(ctx, 20);
                nk_layout_row_template_push_static(ctx, 180);
                nk_layout_row_template_push_dynamic(ctx);
                nk_layout_row_template_push_static(ctx, 180);
                nk_layout_row_template_end(ctx);

                /* Option inputs, locked once it's side connects */
                if (receiver_connected) {
                        nk_widget_disable_begin(ctx);
                }
                nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, receiver_options, sizeof(receiver_options), nk_filter_default);
                if (receiver_connected) {
                        nk_widget_disable_end(ctx);
                }
                nk_spacer(ctx);
                if (sender_connected) {
                        nk_widget_disable_begin(ctx);
                }
                nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, sender_options, sizeof(sender_options), nk_filter_default);
                if (sender_connected) {
                        nk_widget_disable_end(ctx);
                }

                /* Vertical spacer */
                nk_layout_row_dynamic(ctx, 10, 1);

                /* Status line (idle / waiting / transfurring / error) */
                nk_layout_row_dynamic(ctx, 20, 1);
                if (worker_state == wk_error) {
                        nk_label_colored(ctx, status_text(status), NK_TEXT_CENTERED, nk_rgb(255, 80, 80));
                } else if (worker_state == wk_done) {
                        nk_label_colored(ctx, status_text(status), NK_TEXT_CENTERED, nk_rgb(80, 220, 80));
                } else if (worker_active()) {
                        nk_label_colored(ctx, status_text(status), NK_TEXT_CENTERED, nk_rgb(255, 200, 80));
                } else {
                        nk_label_colored(ctx, status_text(status), NK_TEXT_CENTERED, nk_rgb(180, 180, 180));
                }

                /* Row for loading bar */
                nk_layout_row_dynamic(ctx, 20, 1);

                /* Pulse the bar while worker is busy and fill once transfur is complete */
                bar_visible = worker_active() || worker_state == wk_done;
                
                if (bar_visible) {
                        if (worker_state == wk_done) {
                                loading_bar_state = 100;
                        } else {
                                loading_bar_state += 5;
                                if (loading_bar_state > 100) {
                                        loading_bar_state = 0;
                                }
                        }
                        nk_progress(ctx, &loading_bar_state, 100, nk_false);
                }
        }
        nk_end(ctx);
}

int run_gui(const char *title) { return run_gui_nuklear(title, nk_gui); }