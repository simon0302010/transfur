#include "app.h"
#include "../core/widget.h"
#include "../core/widgets/button.h"
#include "../core/widgets/group.h"
#include "../core/widgets/loading_bar.h"
#include "../core/widgets/progress_bar.h"
#include "../core/widgets/split.h"
#include "../core/widgets/text.h"
#include "../core/widgets/text_input.h"

#define APP_WIDGET_COUNT 3

static struct widget widgets[APP_WIDGET_COUNT];
static struct widget recv_group[3];
static struct widget send_group[3];

static struct widget main_split[3];

/* receiving group */
static struct widget_button_options recv_button_opts;
static char recv_button_text[] = "No receiving interface";

static struct widget_text_input_options recv_input_opts;
static char recv_input_text[] = "Input";
static char recv_input_buffer[50];

static struct widget_button_options recv_close_button_opts;
static char recv_close_button_text[] = "Close receiver";

/* middle button */
static struct widget_button_options mid_button_opts;
static char mid_button_text[] = "Initialize Transaction";

/* sending group */
static struct widget_button_options send_button_opts;
static char send_button_text[] = "No sending interface";

static struct widget_text_input_options send_input_opts;
static char send_input_text[] = "Input";
static char send_input_buffer[50];

static struct widget_button_options send_close_button_opts;
static char send_close_button_text[] = "Close sender";

/* below the main split */
static char status[] = "Status: Idle";

static struct widget_progress_bar_options progress_bar_opts;
static char progress_bar_text[] = "Progress";
static float progress_bar_value = 0.0f;

void app_init(struct ui_context *ctx) {
        recv_button_opts.title = recv_button_text;
        recv_input_opts.buffer = recv_input_buffer;
        recv_input_opts.max_len = 50;
        recv_input_opts.cursor = 0;
        recv_input_opts.label = recv_input_text;
        recv_close_button_opts.title = recv_close_button_text;

        recv_group[0] =
            widget_button(&recv_button_opts, sizeof(recv_button_opts));
        recv_group[1] =
            widget_text_input(&recv_input_opts, sizeof(recv_input_opts));
        recv_group[2] = widget_button(&recv_close_button_opts,
                                      sizeof(recv_close_button_opts));

        mid_button_opts.title = mid_button_text;

        send_button_opts.title = send_button_text;
        send_input_opts.buffer = send_input_buffer;
        send_input_opts.max_len = 50;
        send_input_opts.cursor = 0;
        send_input_opts.label = send_input_text;
        send_close_button_opts.title = send_close_button_text;

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
