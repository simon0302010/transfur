#include <stddef.h>

enum RenderableType {
        RENDERABLE_GROUP,
        TEXT,
        PROGRESS_BAR
};

struct Renderable {
        enum RenderableType type;
        size_t content_size;
        void *content;
};

struct ProgressBarOptions {
        int *progress; /* out of 100 */
        char *title;
};

void renderContent(struct Renderable renderable[], size_t count);


struct Renderable createText(int length, char *content);

struct Renderable createGroup(struct Renderable *children, size_t count);

struct Renderable createProgressBar(struct ProgressBarOptions *options, size_t size);
