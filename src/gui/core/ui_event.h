enum ui_key {
  UI_KEY_NONE,
  UI_KEY_CHAR,
  UI_KEY_ENTER,
  UI_KEY_BACKSPACE,
  UI_KEY_TAB,
  UI_KEY_SHIFT_TAB,
  UI_KEY_ESC,
  UI_KEY_LEFT,
  UI_KEY_RIGHT,
  UI_KEY_UP,
  UI_KEY_DOWN
};

struct ui_event {
  enum ui_key key;
  char ch;
  int mouse_x; /* always 0 in TUI environment */
  int mouse_y;
}
