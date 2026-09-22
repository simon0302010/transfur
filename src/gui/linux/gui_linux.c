/*
Source file for Linux GUI code.
Needs to implement all functions defined in `gui.h`
*/

#include <X11/X.h>
#define _POSIX_C_SOURCE 199309L

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_XLIB_IMPLEMENTATION
#include "../nuklear/nuklear.h"
#include "nuklear_xlib.h"

#define DTIME           20
#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600

typedef struct XWindow XWindow;
struct XWindow {
    Display *dpy;
    Window root;
    Visual *vis;
    Colormap cmap;
    XWindowAttributes attr;
    XSetWindowAttributes swa;
    Window win;
    int screen;
    XFont *font;
    unsigned int width;
    unsigned int height;
    Atom wm_delete_window;
};

static void
die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputs("\n", stderr);
    exit(EXIT_FAILURE);
}

static long
timestamp(void)
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0) return 0;
    return (long)((long)tv.tv_sec * 1000 + (long)tv.tv_usec/1000);
}

static void
sleep_for(long t)
{
    struct timespec req;
    const time_t sec = (int)(t/1000);
    const long ms = t - (sec * 1000);
    req.tv_sec = sec;
    req.tv_nsec = ms * 1000000L;
    while(-1 == nanosleep(&req, &req));
}

static void calculator(struct nk_context *ctx)
{
    if (nk_begin(ctx, "Calculator", nk_rect(10, 10, 180, 250),
        NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_MOVABLE))
    {
        static int set = 0, prev = 0, op = 0;
        static const char numbers[] = "789456123";
        static const char ops[] = "+-*/";
        static double a = 0, b = 0;
        static double *current = &a;

        size_t i = 0;
        int solve = 0;
        {int len; char buffer[256];
        nk_layout_row_dynamic(ctx, 35, 1);
        len = sprintf(buffer, "%.2f", *current);
        nk_edit_string(ctx, NK_EDIT_SIMPLE, buffer, &len, 255, nk_filter_float);
        buffer[len] = 0;
        *current = atof(buffer);}

        nk_layout_row_dynamic(ctx, 35, 4);
        for (i = 0; i < 16; ++i) {
            if (i >= 12 && i < 15) {
                if (i > 12) continue;
                if (nk_button_label(ctx, "C")) {
                    a = b = op = 0; current = &a; set = 0;
                } if (nk_button_label(ctx, "0")) {
                    *current = *current*10.0f; set = 0;
                } if (nk_button_label(ctx, "=")) {
                    solve = 1; prev = op; op = 0;
                }
            } else if (((i+1) % 4)) {
                if (nk_button_text(ctx, &numbers[(i/4)*3+i%4], 1)) {
                    *current = *current * 10.0f + numbers[(i/4)*3+i%4] - '0';
                    set = 0;
                }
            } else if (nk_button_text(ctx, &ops[i/4], 1)) {
                if (!set) {
                    if (current != &b) {
                        current = &b;
                    } else {
                        prev = op;
                        solve = 1;
                    }
                }
                op = ops[i/4];
                set = 1;
            }
        }
        if (solve) {
            if (prev == '+') a = a + b;
            if (prev == '-') a = a - b;
            if (prev == '*') a = a * b;
            if (prev == '/') a = a / b;
            current = &a;
            if (set) current = &b;
            b = 0; set = 0;
        }
    }
    nk_end(ctx);
}


int create_window(const char *title)
{
    long dt;
    long started;
    int running = 1;
    XWindow xw;
    struct nk_context *ctx;

    /* X11 */
    memset(&xw, 0, sizeof xw);
    xw.dpy = XOpenDisplay(NULL);
    if (!xw.dpy) die("Could not open a display; perhaps $DISPLAY is not set?");
    xw.root = DefaultRootWindow(xw.dpy);
    xw.screen = XDefaultScreen(xw.dpy);
    xw.vis = XDefaultVisual(xw.dpy, xw.screen);
    xw.cmap = XCreateColormap(xw.dpy,xw.root,xw.vis,AllocNone);

    xw.swa.colormap = xw.cmap;
    xw.swa.event_mask =
        ExposureMask | KeyPressMask | KeyReleaseMask |
        ButtonPress | ButtonReleaseMask| ButtonMotionMask |
        Button1MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask|
        PointerMotionMask | KeymapStateMask;
    xw.win = XCreateWindow(xw.dpy, xw.root, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0,
        XDefaultDepth(xw.dpy, xw.screen), InputOutput,
        xw.vis, CWEventMask | CWColormap, &xw.swa);

    XStoreName(xw.dpy, xw.win, "X11");
    XMapWindow(xw.dpy, xw.win);
    xw.wm_delete_window = XInternAtom(xw.dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(xw.dpy, xw.win, &xw.wm_delete_window, 1);
    XGetWindowAttributes(xw.dpy, xw.win, &xw.attr);
    xw.width = (unsigned int)xw.attr.width;
    xw.height = (unsigned int)xw.attr.height;

    xw.font = nk_xfont_create(xw.dpy, "fixed");
    ctx = nk_xlib_init(xw.font, xw.dpy, xw.screen, xw.win, xw.width, xw.height);

    while (running)
    {
        /* Input */
        XEvent evt;
        started = timestamp();
        nk_input_begin(ctx);
        while (XPending(xw.dpy)) {
            XNextEvent(xw.dpy, &evt);
            if (evt.type == ClientMessage) goto cleanup;
            if (evt.type == KeyPress) {
                int ret;
                KeySym *code = XGetKeyboardMapping(xlib.surf->dpy, (KeyCode)evt.xkey.keycode, 1, &ret);
                if (*code == 'q' && (evt.xkey.state & ControlMask)) goto cleanup;
            }
            if (evt.type == ConfigureNotify) {
                xw.width = (unsigned int)evt.xconfigure.width;
                xw.height = (unsigned int)evt.xconfigure.height;

                nk_window_set_bounds(ctx, title, nk_rect(0, 0, (float)xw.width, (float)xw.height));
            }
            if (XFilterEvent(&evt, xw.win)) continue;
            nk_xlib_handle_event(xw.dpy, xw.screen, xw.win, &evt);
        }
        nk_input_end(ctx);

        /* GUI */
        if (nk_begin(ctx, title, nk_rect(0, 0, xw.width, xw.height), NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BACKGROUND))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");
            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);
        }
        nk_end(ctx);
        if (nk_window_is_hidden(ctx, title)) break;
        /* Draw */
        XClearWindow(xw.dpy, xw.win);
        nk_xlib_render(xw.win, nk_rgb(30,30,30));
        XFlush(xw.dpy);

        /* Timing */
        dt = timestamp() - started;
        if (dt < DTIME)
            sleep_for(DTIME - dt);
    }

cleanup:
    nk_xfont_del(xw.dpy, xw.font);
    nk_xlib_shutdown();
    XUnmapWindow(xw.dpy, xw.win);
    XFreeColormap(xw.dpy, xw.cmap);
    XDestroyWindow(xw.dpy, xw.win);
    XCloseDisplay(xw.dpy);
    return 0;
}