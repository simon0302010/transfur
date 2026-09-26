#include <stddef.h>

enum RenderableType {
        RENDERABLE_GROUP,
        TEXT
};

struct Renderable {
        enum RenderableType type;
        size_t content_size;
        void *content;
};

void renderContent(struct Renderable renderable[], size_t count);


struct Renderable createText(int length, char *content);

struct Renderable createGroup(struct Renderable *children, size_t count)