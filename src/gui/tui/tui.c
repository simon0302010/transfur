#include "tui.h"
#include "raw.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../misc/bool.h"
#include "../../misc/console.h"
#include "../../misc/ttime.h"
#if !defined(_WIN32) && !defined(__MSDOS__)
#include "raw.h"
#endif

#include "input.h"

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
        /* Works on Windows, MS-DOS and POSIX systems */
        width = get_console_size().columns;
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
                case BUTTON: {
                        struct button_options *options = renderables[i].content;

                        if (options->is_focused) {
                                printf("[%s]", options->label);
                        } else {
                                printf("|%s|", options->label);
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
                        if (len > 0)
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
        } else if (renderable.type == BUTTON) {
                struct button_options *options = renderable.content;

                /* Click callback */
                if (event.key == KEY_ENTER)
                        options->on_click();
        }
        return false;
}

void run_tui(struct renderable renderables[], size_t count) {
        tbool running = true;
        char ch;
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

#if !defined(_WIN32) && !defined(__MSDOS__)
        enable_raw_mode();
#endif

        prev_time = get_unix_time();

        while (running) {
                current_time = get_unix_time();
                dt = current_time - prev_time;
                prev_time = current_time;

                update_content(renderables, count, dt);

                printf("\x1b[H"); /* TODO: double check this is supported on all
                                     systems */
                fflush(stdout);

                render_content(renderables, count);

                elapsed = get_unix_time() - current_time;
                if (elapsed < target_frame_duration) {
                        timeout_ms =
                            (int)((target_frame_duration - elapsed) * 1000.0);
                } else {
                        timeout_ms = 0;
                }

                event = poll_key_event(timeout_ms);
                if (event.key != KEY_NONE) {
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

                                        /* Make sure this is foucsable */
                                        /* TODO: Find a better way to check this
                                         * when expanding the list of widgets */
                                        if (renderables[new_focus].type ==
                                                TEXT_INPUT ||
                                            renderables[new_focus].type ==
                                                BUTTON)
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
                                } else if (renderables[focus_index].type ==
                                           BUTTON) {
                                        ((struct button_options *)
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
                                } else if (renderables[new_focus].type ==
                                           BUTTON) {
                                        ((struct button_options *)
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
#if !defined(_WIN32) && !defined(__MSDOS__)
        disable_raw_mode();
#endif
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

struct renderable create_button(struct button_options *options, size_t size) {
        struct renderable new_button;

        new_button.type = BUTTON;

        new_button.content_size = size;

        new_button.content = options;

        return new_button;
}