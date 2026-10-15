#include "basic_wrapper.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static SDL_Window *s_window = NULL;
static SDL_Renderer *s_renderer = NULL;

static SDL_Texture *s_texture = NULL;
static unsigned long *s_framebuffer = NULL;
static int s_width = 0;
static int s_height = 0;

int basic_init(const char *title, int width, int height) {
        SDL_Init(SDL_INIT_VIDEO);

        SDL_CreateWindowAndRenderer(title, width, height, 0, &s_window,
                                    &s_renderer);

        s_width = width;
        s_height = height;

        s_framebuffer =
            (unsigned long *)malloc(width * height * sizeof(unsigned long));

        if (!s_framebuffer)
                return -1;

        s_texture =
            SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_XRGB8888,
                              SDL_TEXTUREACCESS_STREAMING, width, height);

        return 0;
}

void basic_cleanup(void) {
        if (s_framebuffer) {
                free(s_framebuffer);
                s_framebuffer = NULL;
        }

        if (s_texture) {
                SDL_DestroyTexture(s_texture);
                s_texture = NULL;
        }

        SDL_DestroyRenderer(s_renderer);
        s_renderer = NULL;

        SDL_DestroyWindow(s_window);
        s_window = NULL;

        SDL_Quit();
}

void basic_clear(struct color color) {
        int i, total;
        unsigned long pixel;

        if (!s_framebuffer)
                return;

        pixel = ((unsigned long)(unsigned char)color.r << 16) |
                ((unsigned long)(unsigned char)color.g << 8) |
                (unsigned long)(unsigned char)color.b;

        total = s_width * s_height;
        for (i = 0; i < total; i++) {
                s_framebuffer[i] = pixel;
        }
}

void basic_begin_frame(void) {
        /* Default background */
        struct color bg;
        bg.r = 20;
        bg.g = 10;
        bg.b = 40;
        basic_clear(bg);
}

void basic_present(void) {
        if (s_texture && s_framebuffer) {
                SDL_UpdateTexture(s_texture, NULL, s_framebuffer,
                                  s_width * sizeof(unsigned long));
                SDL_RenderTexture(s_renderer, s_texture, NULL, NULL);
        }
        SDL_RenderPresent(s_renderer);
}

void basic_set_pixel(int x, int y, struct color color) {
        if (x >= 0 && x < s_width && y >= 0 && y < s_height && s_framebuffer) {
                s_framebuffer[y * s_width + x] =
                    ((unsigned long)(unsigned char)color.r << 16) |
                    ((unsigned long)(unsigned char)color.g << 8) |
                    (unsigned long)(unsigned char)color.b;
        }
}

/* note: these functions could be moved out of the wrapper */
void basic_fill_rect(int x, int y, int w, int h, struct color color) {
        int row, col;
        int x1, y1, x2, y2;
        unsigned long pixel;

        if (!s_framebuffer || w <= 0 || h <= 0)
                return;

        /* this will ensure we draw within screen boundary */
        x1 = (x < 0) ? 0 : x;
        y1 = (y < 0) ? 0 : y;
        x2 = (x + w > s_width) ? s_width : x + w;
        y2 = (y + h > s_height) ? s_height : y + h;

        if (x1 >= x2 || y1 >= y2)
                return;

        pixel = ((unsigned long)(unsigned char)color.r << 16) |
                ((unsigned long)(unsigned char)color.g << 8) |
                (unsigned long)(unsigned char)color.b;

        for (row = y1; row < y2; row++) {
                int row_offset = row * s_width;
                for (col = x1; col < x2; col++) {
                        s_framebuffer[row_offset + col] = pixel;
                }
        }
}

void basic_draw_rect(int x, int y, int w, int h, struct color color) {
        if (w <= 0 || h <= 0)
                return;

        basic_fill_rect(x, y, w, 1, color);
        basic_fill_rect(x, y + h - 1, w, 1, color);
        basic_fill_rect(x, y, 1, h, color);
        basic_fill_rect(x + w - 1, y, 1, h, color);
}

double basic_get_time(void) { return (double)SDL_GetTicksNS() / 1e9; }

tbool basic_poll_event(struct basic_event *event) {
        SDL_Event sdl_event;

        event->type = BASIC_EVENT_NONE;
        event->mouse_x = 0;
        event->mouse_y = 0;
        event->mouse_button = 0;
        event->key = BASIC_KEY_NONE;
        event->ch = '\0';

        if (!SDL_PollEvent(&sdl_event))
                return false;

        switch (sdl_event.type) {
        case SDL_EVENT_QUIT:
                event->type = BASIC_EVENT_QUIT;
                break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
                event->type = BASIC_EVENT_MOUSE_DOWN;
                event->mouse_x = (int)sdl_event.button.x;
                event->mouse_y = (int)sdl_event.button.y;
                event->mouse_button = (int)sdl_event.button.button;
                break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
                event->type = BASIC_EVENT_MOUSE_UP;
                event->mouse_x = (int)sdl_event.button.x;
                event->mouse_y = (int)sdl_event.button.y;
                event->mouse_button = (int)sdl_event.button.button;
                break;

        case SDL_EVENT_MOUSE_MOTION:
                event->type = BASIC_EVENT_MOUSE_MOVE;
                event->mouse_x = (int)sdl_event.button.x;
                event->mouse_y = (int)sdl_event.button.y;
                break;

        case SDL_EVENT_KEY_DOWN:
                event->type = BASIC_EVENT_KEY_DOWN;

                switch (sdl_event.key.key) {
                case SDLK_ESCAPE:
                        event->key = BASIC_KEY_ESC;
                        break;
                case SDLK_TAB:
                        if (sdl_event.key.mod & SDL_KMOD_SHIFT)
                                event->key = BASIC_KEY_SHIFT_TAB;
                        else
                                event->key = BASIC_KEY_TAB;
                        break;
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                        event->key = BASIC_KEY_ENTER;
                        break;
                case SDLK_BACKSPACE:
                        event->key = BASIC_KEY_BACKSPACE;
                        break;
                case SDLK_DELETE:
                        event->key = BASIC_KEY_DELETE;
                        break;
                case SDLK_UP:
                        event->key = BASIC_KEY_UP;
                        break;
                case SDLK_DOWN:
                        event->key = BASIC_KEY_DOWN;
                        break;
                case SDLK_LEFT:
                        event->key = BASIC_KEY_LEFT;
                        break;
                case SDLK_RIGHT:
                        event->key = BASIC_KEY_RIGHT;
                        break;
                default:
                        event->key = BASIC_KEY_NONE;
                        break;
                }

                if (sdl_event.key.key >= 32 && sdl_event.key.key <= 126) {
                        event->ch = (char)sdl_event.key.key;
                }
                break;

        default:
                break;
        }

        return true;
}
