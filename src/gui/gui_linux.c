/*
Source file for Linux GUI code.
Needs to implement all functions defined in `gui.h`
*/

#include <stdlib.h>
#define NK_IMPLEMENTATION
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear/nuklear.h"

struct nk_context ctx;

int create_window(const char *title)
{
        nk_init(&ctx, NULL, NULL);
        if (nk_begin(&ctx, "Transfur", nk_rect(10, 10, 400, 300), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE))
        {
                
        }
}