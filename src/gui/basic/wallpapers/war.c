#include "war.h"
#include "../wallpaper.h"
#include "../wrappers/basic_wrapper.h"
#include <stdlib.h>

#define WAR_MIN_CYCLES 50
#define WAR_MAX_CYCLES 250

#define WAR_MIN_SCALE 2
#define WAR_MAX_SCALE 8

#define WAR_MIN_TEAMS 2
#define WAR_MAX_TEAMS 12

struct color war_colors[] = {{255, 0, 0},   {0, 255, 0},    {0, 0, 255},
                             {225, 225, 0}, {255, 45, 128}, {220, 20, 95},
                             {255, 95, 31}, {255, 200, 0},  {34, 215, 122},
                             {16, 168, 92}, {162, 230, 65}, {0, 210, 255}};

/* How to fight a war:
- each round, each team will grow it's land by 1 pixel in each direction
- if a team tries to grow into another team's land, the outcome is random
*/

/* Lasers mode:
if enabled:
- on it's turn, a pixel has a low chance to shoot a laser instead
- it will shoot to a random pixel
- that pixel must be empty, or else this fails and the shooter pixel loses its
turn
- if the laser hits an empty pixel, it will create a new pixel of the shooter's
team
*/

#define WAR_LASER_CHANCE 0.1f

void basic_draw_war_wallpaper(struct color *wallpaper_pixels,
                              unsigned int width, unsigned int height,
                              tbool lasers_enabled) {
        unsigned int scale =
            basic_wallpaper_get_random() * (WAR_MAX_SCALE - WAR_MIN_SCALE) +
            WAR_MIN_SCALE;
        unsigned int num_teams =
            basic_wallpaper_get_random() * (WAR_MAX_TEAMS - WAR_MIN_TEAMS) +
            WAR_MIN_TEAMS;

        unsigned int scale_width = width / scale;
        unsigned int scale_height = height / scale;

        unsigned int i, y, x, sy, sx;

        int num_cycles =
            basic_wallpaper_get_random() * (WAR_MAX_CYCLES - WAR_MIN_CYCLES) +
            WAR_MIN_CYCLES;

        /* let 0 = no team, any other num corresponds to team + 1 */
        unsigned int *battlefield = (unsigned int *)calloc(
            scale_width * scale_height, sizeof(unsigned int));

        /* add starting team pixels */
        for (i = 0; i < num_teams; i++) {
                unsigned int x =
                    basic_wallpaper_get_random() * (scale_width - 1);
                unsigned int y =
                    basic_wallpaper_get_random() * (scale_height - 1);
                battlefield[y * scale_width + x] = i + 1;
        }

        /* reset wallpaper_pixels to black */
        for (sy = 0; sy < height; sy++) {
                for (sx = 0; sx < width; sx++) {
                        wallpaper_pixels[sy * width + sx].r = 0;
                        wallpaper_pixels[sy * width + sx].b = 0;
                        wallpaper_pixels[sy * width + sx].g = 0;
                }
        }

        for (i = 0; i < num_cycles; i++) {
                for (y = 0; y < scale_height; y++) {
                        for (x = 0; x < scale_width; x++) {
                                unsigned int team =
                                    battlefield[y * scale_width + x];
                                unsigned int neighbor_team;
                                if (team == 0)
                                        continue;
                                if (lasers_enabled &&
                                    basic_wallpaper_get_random() <
                                        WAR_LASER_CHANCE) {
                                        unsigned int target_x =
                                            basic_wallpaper_get_random() *
                                            (scale_width - 1);
                                        unsigned int target_y =
                                            basic_wallpaper_get_random() *
                                            (scale_height - 1);
                                        if (battlefield[target_y * scale_width +
                                                        target_x] == 0) {
                                                battlefield[target_y *
                                                                scale_width +
                                                            target_x] = team;
                                        }
                                        continue;
                                }
                                if (y > 0) {
                                        if (x > 0) {
                                                neighbor_team = battlefield
                                                    [(y - 1) * scale_width +
                                                     (x - 1)];
                                                if (neighbor_team == 0) {
                                                        battlefield
                                                            [(y - 1) *
                                                                 scale_width +
                                                             (x - 1)] = team;
                                                } else if (neighbor_team !=
                                                           team) {
                                                        if (basic_wallpaper_get_random() <
                                                            0.5) {
                                                                battlefield
                                                                    [(y - 1) *
                                                                         scale_width +
                                                                     (x - 1)] =
                                                                        team;
                                                        } else {
                                                                battlefield
                                                                    [y * scale_width +
                                                                     x] =
                                                                        neighbor_team;
                                                        }
                                                }
                                        }
                                        if (x + 1 < scale_width) {
                                                neighbor_team = battlefield
                                                    [(y - 1) * scale_width +
                                                     (x + 1)];
                                                if (neighbor_team == 0) {
                                                        battlefield
                                                            [(y - 1) *
                                                                 scale_width +
                                                             (x + 1)] = team;
                                                } else if (neighbor_team !=
                                                           team) {
                                                        if (basic_wallpaper_get_random() <
                                                            0.5) {
                                                                battlefield
                                                                    [(y - 1) *
                                                                         scale_width +
                                                                     (x + 1)] =
                                                                        team;
                                                        } else {
                                                                battlefield
                                                                    [y * scale_width +
                                                                     x] =
                                                                        neighbor_team;
                                                        }
                                                }
                                        }
                                }
                                if (y + 1 < scale_height) {
                                        if (x > 0) {
                                                neighbor_team = battlefield
                                                    [(y + 1) * scale_width +
                                                     (x - 1)];
                                                if (neighbor_team == 0) {
                                                        battlefield
                                                            [(y + 1) *
                                                                 scale_width +
                                                             (x - 1)] = team;
                                                } else if (neighbor_team !=
                                                           team) {
                                                        if (basic_wallpaper_get_random() <
                                                            0.5) {
                                                                battlefield
                                                                    [(y + 1) *
                                                                         scale_width +
                                                                     (x - 1)] =
                                                                        team;
                                                        } else {
                                                                battlefield
                                                                    [y * scale_width +
                                                                     x] =
                                                                        neighbor_team;
                                                        }
                                                }
                                        }
                                        if (x + 1 < scale_width) {
                                                neighbor_team = battlefield
                                                    [(y + 1) * scale_width +
                                                     (x + 1)];
                                                if (neighbor_team == 0) {
                                                        battlefield
                                                            [(y + 1) *
                                                                 scale_width +
                                                             (x + 1)] = team;
                                                } else if (neighbor_team !=
                                                           team) {
                                                        if (basic_wallpaper_get_random() <
                                                            0.5) {
                                                                battlefield
                                                                    [(y + 1) *
                                                                         scale_width +
                                                                     (x + 1)] =
                                                                        team;
                                                        } else {
                                                                battlefield
                                                                    [y * scale_width +
                                                                     x] =
                                                                        neighbor_team;
                                                        }
                                                }
                                        }
                                }
                        }
                }

                /* draw battlefield */
                if (i % 100 == 0) {
                        for (y = 0; y < scale_height; y++) {
                                for (x = 0; x < scale_width; x++) {
                                        struct color existing_color =
                                            wallpaper_pixels[(y * scale) *
                                                                 width +
                                                             (x * scale)];
                                        struct color new_color;
                                        unsigned int team =
                                            battlefield[(y)*scale_width + (x)];
                                        if (team == 0) {
                                                new_color.r = 0;
                                                new_color.g = 0;
                                                new_color.b = 0;
                                        } else {
                                                new_color =
                                                    war_colors[team - 1];
                                        }
                                        new_color.r =
                                            (existing_color.r + new_color.r) /
                                            2;
                                        new_color.g =
                                            (existing_color.g + new_color.g) /
                                            2;
                                        new_color.b =
                                            (existing_color.b + new_color.b) /
                                            2;
                                        for (sy = 0; sy < scale; sy++) {
                                                for (sx = 0; sx < scale; sx++) {
                                                        if (y + sy < height &&
                                                            x + sx < width) {
                                                                wallpaper_pixels
                                                                    [(y * scale +
                                                                      sy) *
                                                                         width +
                                                                     (x * scale +
                                                                      sx)] =
                                                                        new_color;
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }

        free(battlefield);
}