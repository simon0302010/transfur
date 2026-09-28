#include "tui.h"
#include "raw.h"
#include <poll.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../../misc/bool.h"
#include "raw.h"

/* #define WIDTH_OVERRIDE 0 */
#define USE_ANSI

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

void render_content(struct renderable renderables[], size_t count) {
        int width;
        int i;

#ifdef WIDTH_OVERRIDE
        width = WIDTH_OVERRIDE;
#else
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        width = w.ws_col;
#endif

        for (i = 0; i < count; i++) {
                switch (renderables[i].type) {
                case TEXT:
                        printf("%s\n", (char *)renderables[i].content);
                        fflush(stdout);
                        break;
                case RENDERABLE_GROUP:
                        render_content(renderables[i].content,
                                       renderables[i].content_size);
                        break;
                case PROGRESS_BAR: {
                        /*
                        TODO: Add alignment of options and multiple renderables
                        in a row.
                        */

                        struct progress_bar_options *options =
                            renderables[i].content;
                        int heading_chars;
                        int progress_chars;
                        int remaining_width;
                        char progress_string[8];
                        /* Turns XX% into a string */
                        sprintf(progress_string, "%i%%",
                                (int)(*(options->progress) * 100));

                        /* Calculate the widths now that we have progress_string
                         */
                        heading_chars = strlen(options->title) +
                                        strlen(progress_string) + 4;
                        progress_chars =
                            (*(options->progress)) * (width - heading_chars);
                        remaining_width =
                            width - heading_chars - progress_chars;

#ifdef USE_ANSI
                        printf("%s|%s%s%s|%s%s%s|%s", ANSI_COLOR_CYAN,
                               ANSI_COLOR_YELLOW, options->title,
                               ANSI_COLOR_CYAN, ANSI_COLOR_MAGENTA,
                               progress_string, ANSI_COLOR_CYAN,
                               ANSI_COLOR_GREEN);
#else
                        printf("|%s|%s|", options->title, progress_string);
#endif

                        for (; progress_chars > 0; progress_chars--) {
                                printf("%%");
                        }

#ifdef USE_ANSI
                        printf(ANSI_COLOR_RESET);
#endif

                        for (; remaining_width > 0; remaining_width--) {
                                printf("-");
                        }

#ifdef USE_ANSI
                        printf(ANSI_COLOR_CYAN);
#endif

                        printf("|\n");

#ifdef USE_ANSI
                        printf(ANSI_COLOR_RESET);
#endif

                        break;
                }

                        fflush(stdout);

                        break;
                case LOADING_BAR: {
                        struct loading_bar_options *options =
                            renderables[i].content;

                        short i;
                        short charset_size = 6;
                        const char charset[6] = {'%', '$', '&', '-', '-', '-'};
                        size_t title_width = strlen(options->title);
                        int bar_width = width - (int)title_width - 3;
                        char *loading_bar;

                        if (bar_width <= 0)
                                break;

                        loading_bar = malloc((size_t)bar_width + 1);
                        if (!loading_bar)
                                break;

                        for (i = 0; i < bar_width; i++)
                                loading_bar[i] = charset[(i + options->offset) %
                                                         charset_size];

                        loading_bar[bar_width] = '\0';

#ifdef USE_ANSI
                        printf("%s|%s%s%s|%s%s%s|%s\n", ANSI_COLOR_CYAN,
                               ANSI_COLOR_YELLOW, options->title,
                               ANSI_COLOR_CYAN, ANSI_COLOR_GREEN, loading_bar,
                               ANSI_COLOR_CYAN, ANSI_COLOR_RESET);
#else
                        printf("|%s|%s|\n", options->title, loading_bar);
#endif
                        fflush(stdout);

                        free(loading_bar);

                        /* offset updated in update_content */

                        break;
                }
                case TEXT_INPUT: {
                        struct text_input_options *options =
                            renderables[i].content;

                        if (options->is_focused) {
                                /* TODO: Account for cursor position */
                                printf("%s [%s_]", options->label,
                                       options->buffer);

                        } else {
                                printf("%s [%s]", options->label,
                                       options->buffer);
                        }

                        break;
                }
                }
        }
}

void update_content(struct renderable renderables[], size_t count, double dt) {
        size_t i;
        const short charset_size = 6;
        for (i = 0; i < count; i++) {
                switch (renderables[i].type) {
                case LOADING_BAR: {
                        struct loading_bar_options *options =
                            renderables[i].content;
                        float step_interval;
                        if (options->speed <= 0.0f) {
                                break;
                        }
                        step_interval = 1.0f / options->speed;
                        options->accumulator += (float)dt;
                        while (options->accumulator >= step_interval) {
                                options->offset =
                                    (options->offset + 1) % charset_size;
                                options->accumulator -= step_interval;
                        }
                        break;
                }
                case RENDERABLE_GROUP:
                        update_content(renderables[i].content,
                                       renderables[i].content_size, dt);
                        break;
                default:
                        break;
                }
        }
}

static struct tui_event read_key_event(void) {
        struct tui_event event;
        char ch;

        event.key = KEY_NONE;
        event.ch = '\0';

        if (read(STDIN_FILENO, &ch, 1) <= 0) {
                return event;
        }

        if (ch == '\r' || ch == '\n') {
                event.key = KEY_ENTER;
        } else if (ch == '\t') {
                event.key = KEY_TAB;
        } else if (ch == 127 || ch == '\b') {
                event.key = KEY_BACKSPACE;
        } else if (ch == 27) {
                /* esc or an ANSI sequence */
                struct pollfd pfd;
                char seq[3];

                pfd.fd = STDIN_FILENO;
                pfd.events = POLLIN;

                if (poll(&pfd, 1, 25) > 0 && (pfd.revents & POLLIN)) {
                        if (read(STDIN_FILENO, &seq[0], 1) > 0 &&
                            seq[0] == '[') {
                                /* this is an ansi sequence */
                                if (read(STDIN_FILENO, &seq[1], 1) > 0) {
                                        switch (seq[1]) {
                                        case 'A':
                                                event.key = KEY_ARROW_UP;
                                                break;
                                        case 'B':
                                                event.key = KEY_ARROW_DOWN;
                                                break;
                                        case 'C':
                                                event.key = KEY_ARROW_RIGHT;
                                                break;
                                        case 'D':
                                                event.key = KEY_ARROW_LEFT;
                                                break;
                                        case 'Z':
                                                event.key = KEY_SHIFT_TAB;
                                                break;
                                        case '3':
                                                if (read(STDIN_FILENO, &seq[2],
                                                         1) > 0 &&
                                                    seq[2] == '~') {
                                                        event.key = KEY_DELETE;
                                                }
                                                break;
                                        default:
                                                break;
                                        }
                                }
                        }
                } else {
                        event.key = KEY_ESC;
                }
        } else if ((unsigned char)ch >= 32 && (unsigned char)ch <= 126) {
                event.key = KEY_CHAR;
                event.ch = ch;
        }

        return event;
}

tbool renderable_handle_event(struct tui_event event,
                              struct renderable renderable) {
        if (renderable.type == TEXT_INPUT) {
                struct text_input_options *options =
                    (struct text_input_options *)renderable.content;
                size_t len;

                len = strlen(options->buffer);

                switch (event.key) {
                case KEY_CHAR:
                        /* TODO: Acocount for cursor position */
                        /* TODO: account for max len option */
                        options->buffer[len] = event.ch;
                        options->buffer[len + 1] = '\0';
                        break;
                case KEY_BACKSPACE:
                        /* TODO: Acocount for cursor position */
                        options->buffer[len - 1] = '\0';
                        break;

                case KEY_DELETE: /* TODO: handle the below */
                        break;
                case KEY_ARROW_LEFT:
                        break;
                case KEY_ARROW_RIGHT:
                        break;
                default:
                        return false;
                        break;
                }
                return true;
        }
        return false;
}

void run_tui(struct renderable renderables[], size_t count) {
        tbool running = true;
        int ret;
        char ch;
        struct pollfd pfd;
        int timeout_ms;
        double current_time;
        double prev_time;
        double dt;
        double elapsed;
        const double target_fps = 30.0;
        const double target_frame_duration = 1.0 / target_fps;

        /* if -1, nothing is focused */
        int focus_index = -1;
        struct tui_event event;
        tbool handled;
        size_t i;

        enable_raw_mode();

        pfd.fd = STDIN_FILENO;
        pfd.events = POLLIN;

        prev_time = get_time_seconds();

        while (running) {
                current_time = get_time_seconds();
                dt = current_time - prev_time;
                prev_time = current_time;

                update_content(renderables, count, dt);

                printf("\x1b[H"); /* TODO: double check this is supported on all
                                     systems */
                fflush(stdout);

                render_content(renderables, count);

                elapsed = get_time_seconds() - current_time;
                if (elapsed < target_frame_duration) {
                        timeout_ms =
                            (int)((target_frame_duration - elapsed) * 1000.0);
                } else {
                        timeout_ms = 0;
                }

                ret = poll(&pfd, 1, timeout_ms);
                if (ret > 0 && (pfd.revents & POLLIN)) {
                        event = read_key_event(); /* TODO: implement */
                        handled = false;

                        /* navigation keys */
                        if (event.key == KEY_TAB ||
                            event.key == KEY_SHIFT_TAB) {
                                int direction = (event.key == KEY_TAB);
                                int new_focus = focus_index;

                                for (i = 0; i < count; i++) {
                                        new_focus = (new_focus + direction +
                                                     (int)count) %
                                                    (int)count;

                                        /* make sure this is foucsable */
                                        if (renderables[new_focus].type ==
                                            TEXT_INPUT)
                                                break;
                                }

                                /* modify options to set the new one as focused
                                 * and old one as unfocused */
                                if (renderables[focus_index].type ==
                                    TEXT_INPUT) {
                                        ((struct text_input_options *)
                                             renderables[focus_index]
                                                 .content)
                                            ->is_focused = false;
                                }
                                focus_index = new_focus;
                                if (renderables[new_focus].type == TEXT_INPUT) {
                                        ((struct text_input_options *)
                                             renderables[focus_index]
                                                 .content)
                                            ->is_focused = true;
                                }

                                handled = true;
                        }

                        /* TODO: send to focused widget */
                        if (!handled && focus_index >= 0 &&
                            focus_index < (int)count) {
                                handled = renderable_handle_event(
                                    event, renderables[focus_index]);
                        }

                        /* quit keys */
                        if (!handled) {
                                if (event.key == KEY_ESC ||
                                    (event.key == KEY_CHAR &&
                                     event.ch == 'q')) {
                                        running = false;
                                }
                        }
                }
        }

        disable_raw_mode();
}

struct renderable create_text(int length, char *content) {
        struct renderable new_text;

        new_text.type = TEXT;

        new_text.content_size = length + 1;

        new_text.content = content;

        return new_text;
}

struct renderable create_group(struct renderable *children, size_t count) {
        struct renderable new_group;

        new_group.type = RENDERABLE_GROUP;

        new_group.content_size = count;

        new_group.content = children;

        return new_group;
}

struct renderable create_progress_bar(struct progress_bar_options *options,
                                      size_t size) {
        struct renderable new_progress_bar;

        new_progress_bar.type = PROGRESS_BAR;

        new_progress_bar.content_size = size;

        new_progress_bar.content = options;

        return new_progress_bar;
}

struct renderable create_text_input(struct text_input_options *options,
                                    size_t size) {
        struct renderable new_text_input;

        new_text_input.type = TEXT_INPUT;

        new_text_input.content_size = size;

        new_text_input.content = options;

        return new_text_input;
}

struct renderable create_loading_bar(struct loading_bar_options *options,
                                     size_t size) {
        struct renderable new_loading_bar;

        new_loading_bar.type = LOADING_BAR;

        new_loading_bar.content_size = size;

        new_loading_bar.content = options;

        return new_loading_bar;
}