#include "tui.h"
#include <stddef.h>
#include <stdio.h>

void renderContent(struct Renderable renderables[], size_t count) {
        int i;
        for (i = 0; i < count; i++) {
                switch (renderables[i].type) {
                        case TEXT:
                                printf("%s\n", renderables[i].content);
                                break;
                        case RENDERABLE_GROUP:
                                renderContent(renderables[i].content, renderables[i].content_size);
                                break;
                }
        }
}
        

struct Renderable createText(int length, char *content) {
        struct Renderable newText;

        newText.type = TEXT;

        newText.content_size = length + 1;

        newText.content = content;

        return newText;
}

struct Renderable createGroup(struct Renderable *children, size_t count) {
        struct Renderable newGroup;

        newGroup.type = RENDERABLE_GROUP;

        newGroup.content_size = count;

        newGroup.content = children;

        return newGroup;
}
