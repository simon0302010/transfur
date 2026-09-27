struct color {
        int r;
        int g;
        int b;
};

enum basic_event_type {
        BASIC_EVENT_NONE,
        BASIC_EVENT_QUIT,
        BASIC_EVENT_MOUSE_DOWN,
        BASIC_EVENT_MOUSE_UP,
        BASIC_EVENT_MOUSE_MOVE,
        BASIC_EVENT_KEY_DOWN
};

struct basic_event {
        enum basic_event_type type;
        int mouse_x;
        int mouse_y;
        int mouse_button;
        int key_code;
        char ch;
}

int basic_init(const char *title, int width, int height);

void basic_cleanup(void);

void basic_begin_frame(void);

void basic_present(void);

bool basic_poll_event(struct basic_event *event);

double basic_get_time(void);

void basic_clear(struct color color);

void basic_set_pixel(int x, int y, struct color color);

void basic_draw_rect(int x, int y, int w, int h, struct color color);

void basic_fill_rect(int x, int y, int w, int h, struct color color);
