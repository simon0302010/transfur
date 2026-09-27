#include "basic_wrapper.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <stdio.h>

static SDL_Window *s_window = NULL;
static SDL_Renderer *s_renderer = NULL;

int basic_init(const char *title, int width, int height) {
        SDL_Init(SDL_INIT_VIDEO);

        SDL_CreateWindowAndRenderer(title, width, height, 0, &s_window, &s_renderer);

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

void basic_present(void) {
        SDL_RenderPresent(s_renderer);
}

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

double basic_get_time(void) {
        return (double)SDL_GetTicksNS() / 1e9;
}

bool basic_poll_event(struct basic_event *event) {
        SDL_Event sdl_event;

        event->type = BASIC_EVENT_NONE;
        event->mouse_x = 0;
        event->mouse_y = 0;
        event->mouse_button = 0;
        event->key_code = 0;
        event->ch = '\0';

        if (!SDL_PollEvent(&sdl_event)) return false;

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
                        event->key_code = (int)sdl_event.key.key;
                        if (sdl_event.key.key >= 32 && sdl_event.key.key <= 126) {
                                event->ch = (char)sdl_event.key.key;
                        }
                        break;

                default:
                        break;
        }

        return true;
}
