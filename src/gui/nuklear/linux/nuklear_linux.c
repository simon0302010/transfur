/*
Source file for Linux GUI code.
Needs to implement all functions defined in `gui.h`
*/

#include <X11/X.h>
#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_XLIB_IMPLEMENTATION
#include "../../gui.h"
#include "nuklear_xlib.h"

#define DTIME 20

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

static void die(const char *fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
        fputs("\n", stderr);
        exit(EXIT_FAILURE);
}

static long timestamp(void) {
        struct timeval tv;
        if (gettimeofday(&tv, NULL) < 0)
                return 0;
        return (long)((long)tv.tv_sec * 1000 + (long)tv.tv_usec / 1000);
}

static void sleep_for(long t) {
        struct timespec req;
        const time_t sec = (int)(t / 1000);
        const long ms = t - (sec * 1000);
        req.tv_sec = sec;
        req.tv_nsec = ms * 1000000L;
        while (-1 == nanosleep(&req, &req))
                ;
}

int run_gui_nuklear(const char *title,
                    void (*gui)(const char *title, struct nk_context *ctx,
                                int width, int height)) {
        long dt;
        long started;
        int running = 1;
        XWindow xw;
        struct nk_context *ctx;

        /* X11 */
        memset(&xw, 0, sizeof xw);
        xw.dpy = XOpenDisplay(NULL);
        if (!xw.dpy)
                die("Could not open a display; perhaps $DISPLAY is not set?");
        xw.root = DefaultRootWindow(xw.dpy);
        xw.screen = XDefaultScreen(xw.dpy);
        xw.vis = XDefaultVisual(xw.dpy, xw.screen);
        xw.cmap = XCreateColormap(xw.dpy, xw.root, xw.vis, AllocNone);

        xw.swa.colormap = xw.cmap;
        xw.swa.event_mask =
            ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPress |
            ButtonReleaseMask | ButtonMotionMask | Button1MotionMask |
            Button3MotionMask | Button4MotionMask | Button5MotionMask |
            PointerMotionMask | KeymapStateMask | StructureNotifyMask;
        xw.win =
            XCreateWindow(xw.dpy, xw.root, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0,
                          XDefaultDepth(xw.dpy, xw.screen), InputOutput, xw.vis,
                          CWEventMask | CWColormap, &xw.swa);

        XStoreName(xw.dpy, xw.win, "X11");
        XMapWindow(xw.dpy, xw.win);
        xw.wm_delete_window = XInternAtom(xw.dpy, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(xw.dpy, xw.win, &xw.wm_delete_window, 1);
        XGetWindowAttributes(xw.dpy, xw.win, &xw.attr);
        xw.width = (unsigned int)xw.attr.width;
        xw.height = (unsigned int)xw.attr.height;

        xw.font = nk_xfont_create(xw.dpy, "fixed");
        ctx = nk_xlib_init(xw.font, xw.dpy, xw.screen, xw.win, xw.width,
                           xw.height);

        while (running) {
                /* Input */
                XEvent evt;
                started = timestamp();
                nk_input_begin(ctx);
                while (XPending(xw.dpy)) {
                        XNextEvent(xw.dpy, &evt);
                        if (evt.type == ClientMessage)
                                goto cleanup;
                        if (evt.type == KeyPress) {
                                int ret;
                                KeySym *code = XGetKeyboardMapping(
                                    xlib.surf->dpy, (KeyCode)evt.xkey.keycode,
                                    1, &ret);
                                if (*code == 'q' &&
                                    (evt.xkey.state & ControlMask))
                                        goto cleanup;
                        }
                        if (evt.type == ConfigureNotify) {
                                xw.width = (unsigned int)evt.xconfigure.width;
                                xw.height = (unsigned int)evt.xconfigure.height;
                        }
                        if (XFilterEvent(&evt, xw.win))
                                continue;
                        nk_xlib_handle_event(xw.dpy, xw.screen, xw.win, &evt);
                }
                nk_input_end(ctx);

                /* Runs the GUI code */
                gui(title, ctx, xw.width, xw.height);

                if (nk_window_is_hidden(ctx, title))
                        break;
                /* Draw */
                XClearWindow(xw.dpy, xw.win);
                nk_xlib_render(xw.win, nk_rgb(30, 30, 30));
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