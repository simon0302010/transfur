#include "basic_wrapper.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_render.h>
#include <stdio.h>

static SDL_Window *s_window = NULL;
static SDL_Renderer *s_renderer = NULL;

int basic_init(const char *title, int width, int height) {
        SDL_Init(SDL_INIT_VIDEO);

        SDL_CreateWindowAndRenderer(title, width, height, 0, &s_window,
                                    &s_renderer);

        return 0;
}

void basic_cleanup(void) {
        SDL_DestroyRenderer(s_renderer);
        s_renderer = NULL;

        SDL_DestroyWindow(s_window);
        s_window = NULL;

        SDL_Quit();
}

void basic_clear(struct color color) {
        SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, 255);
        SDL_RenderClear(s_renderer);
}

void basic_begin_frame(void) {
        /* Default background */
        struct color bg;
        bg.r = 20;
        bg.g = 10;
        bg.b = 40;
        basic_clear(bg);
}

void basic_present(void) { SDL_RenderPresent(s_renderer); }

void basic_set_pixel(int x, int y, struct color color) {
        SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, 255);
        SDL_RenderPoint(s_renderer, (float)x, (float)y);
}

void basic_draw_rect(int x, int y, int w, int h, struct color color) {
        SDL_FRect rect;

        rect.x = (float)x;
        rect.y = (float)y;
        rect.w = (float)w;
        rect.h = (float)h;

        SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, 255);
        SDL_RenderRect(s_renderer, &rect);
}

void basic_fill_rect(int x, int y, int w, int h, struct color color) {
        SDL_FRect rect;

        rect.x = (float)x;
        rect.y = (float)y;
        rect.w = (float)w;
        rect.h = (float)h;

        SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, 255);
        SDL_RenderFillRect(s_renderer, &rect);
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
