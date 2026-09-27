#include <stddef.h>

enum basic_renderable_type {
        BASIC_PANEL
};

struct basic_renderable {
        enum basic_renderable_type type;
};

void run_basic(struct basic_renderable renderables[], size_t count);