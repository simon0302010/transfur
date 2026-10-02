enum widget_type {
  WIDGET_TEXT,
  WIDGET_PROGRESS_BAR,
  WIDGET_LOADING_BAR,
  WIDGET_TEXT_INPUT,
  WIDGET_GROUP /* this will replace BASIC_PANEL */
}

struct widget {
  enum widget_type type;
  size_t content_size;
  void *content;
}
