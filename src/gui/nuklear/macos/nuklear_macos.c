/* nuklear - 1.32.0 - public domain */
#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "../../gui.h"
#include "nuklear_sdl_renderer.h"

int run_gui_nuklear(const char *title,
                    void (*gui)(const char *title, struct nk_context *ctx,
                                int width, int height)) {
        /* Platform */
        SDL_Window *win;
        SDL_Renderer *renderer;
        int running = 1;
        int flags = 0;
        float font_scale = 1;

        /* GUI */
        struct nk_context *ctx;
        struct nk_colorf bg;

        /* SDL setup */
        SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
        SDL_Init(SDL_INIT_VIDEO);

        win = SDL_CreateWindow("sdl_renderer", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                               WINDOW_HEIGHT,
                               SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

        if (win == NULL) {
                SDL_Log("Error SDL_CreateWindow %s", SDL_GetError());
                exit(-1);
        }

        flags |= SDL_RENDERER_ACCELERATED;
        flags |= SDL_RENDERER_PRESENTVSYNC;

#if 0
    SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengles2");
#endif

        renderer = SDL_CreateRenderer(win, -1, flags);

        if (renderer == NULL) {
                SDL_Log("Error SDL_CreateRenderer %s", SDL_GetError());
                exit(-1);
        }

        /* scale the renderer output for High-DPI displays */
        {
                int render_w, render_h;
                int window_w, window_h;
                float scale_x, scale_y;
                SDL_GetRendererOutputSize(renderer, &render_w, &render_h);
                SDL_GetWindowSize(win, &window_w, &window_h);
                scale_x = (float)(render_w) / (float)(window_w);
                scale_y = (float)(render_h) / (float)(window_h);
                SDL_RenderSetScale(renderer, scale_x, scale_y);
                font_scale = scale_y;
        }

        /* GUI */
        ctx = nk_sdl_init(win, renderer);
        {
                struct nk_font_atlas *atlas;
                struct nk_font_config config = nk_font_config(0);
                struct nk_font *font;

                /* set up the font atlas and add desired font; note that font
                 * sizes are multiplied by font_scale to produce better results
                 * at higher DPIs */
                nk_sdl_font_stash_begin(&atlas);
                font =
                    nk_font_atlas_add_default(atlas, 13 * font_scale, &config);
                /*font = nk_font_atlas_add_from_file(atlas,
                 * "../../../extra_font/DroidSans.ttf", 14 * font_scale,
                 * &config);*/
                /*font = nk_font_atlas_add_from_file(atlas,
                 * "../../../extra_font/Roboto-Regular.ttf", 16 * font_scale,
                 * &config);*/
                /*font = nk_font_atlas_add_from_file(atlas,
                 * "../../../extra_font/kenvector_future_thin.ttf", 13 *
                 * font_scale, &config);*/
                /*font = nk_font_atlas_add_from_file(atlas,
                 * "../../../extra_font/ProggyClean.ttf", 12 * font_scale,
                 * &config);*/
                /*font = nk_font_atlas_add_from_file(atlas,
                 * "../../../extra_font/ProggyTiny.ttf", 10 * font_scale,
                 * &config);*/
                /*font = nk_font_atlas_add_from_file(atlas,
                 * "../../../extra_font/Cousine-Regular.ttf", 13 * font_scale,
                 * &config);*/
                nk_sdl_font_stash_end();

                /* this hack makes the font appear to be scaled down to the
                 * desired size and is only necessary when font_scale > 1 */
                font->handle.height /= font_scale;
                /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
                nk_style_set_font(ctx, &font->handle);
        }

        bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
        while (running) {
                /* Input */
                SDL_Event evt;
                nk_input_begin(ctx);
                while (SDL_PollEvent(&evt)) {
                        if (evt.type == SDL_QUIT)
                                goto cleanup;
                        if (evt.type == SDL_KEYDOWN &&
                            evt.key.keysym.sym == SDLK_q &&
                            SDL_GetModState() & KMOD_CTRL)
                                goto cleanup;
                        nk_sdl_handle_event(&evt);
                }
                nk_sdl_handle_grab(); /* optional grabbing behavior */
                nk_input_end(ctx);

                {
                        int window_w, window_h;
                        SDL_GetWindowSize(win, &window_w, &window_h);
                        gui(title, ctx, window_w, window_h);
                }

                SDL_SetRenderDrawColor(renderer, bg.r * 255, bg.g * 255,
                                       bg.b * 255, bg.a * 255);
                SDL_RenderClear(renderer);

                nk_sdl_render(NK_ANTI_ALIASING_ON);

                SDL_RenderPresent(renderer);
        }

cleanup:
        nk_sdl_shutdown();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 0;
}