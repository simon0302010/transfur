#ifndef CORE_WIDGETS_BUTTON_H
#define CORE_WIDGETS_BUTTON_H

#include "../widget.h"

typedef void (*button_callback_t)(void);

struct widget_button_options {
        char *title;
        button_callback_t callback;
};

struct widget widget_button(struct widget_button_options *opts, size_t size);

#endif /* CORE_WIDGETS_BUTTON_H */
