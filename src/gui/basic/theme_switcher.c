#include "theme_switcher.h"
#include "design.h"
#include "drawing.h"
#include "images.h"
#include "text.h"
#include "theme.h"
#include <math.h>

#define THEME_SWITCHER_SWITCH_TIME 0.4 /* animation between themes */
#define THEME_SWITCHER_HOLD_TIME 1.5   /* how long to wait after no updates */
#define THEME_SWITCHER_FADE_TIME                                               \
        0.4 /* how long to fade after hold time is up */

#define BASIC_MARGIN 8  /* stolen from render.c */
#define BASIC_SPACING 4 /* also stolen from render.c */

static double switcher_timer = 0.0;
static float switcher_alpha = 0.0f;
static float switcher_animation_progress = 0.0f;

void theme_switcher_show(void) {
        switcher_timer = THEME_SWITCHER_SWITCH_TIME + THEME_SWITCHER_HOLD_TIME +
                         THEME_SWITCHER_FADE_TIME;
        switcher_alpha = 1.0f;
        switcher_animation_progress = 0.0f;
}

void theme_switcher_update(double dt) {
        if (switcher_timer <= 0.0) {
                switcher_alpha = 0.0f;
                return;
        }

        switcher_timer -= dt;

        if (switcher_timer <= THEME_SWITCHER_FADE_TIME) {
                switcher_alpha =
                    (float)(switcher_timer / THEME_SWITCHER_FADE_TIME);
        } else if (switcher_timer <=
                   THEME_SWITCHER_FADE_TIME + THEME_SWITCHER_HOLD_TIME) {
                switcher_alpha = 1.0f;
                switcher_animation_progress = 0.0f;
        } else {
                switcher_alpha = 1.0f;
                switcher_animation_progress =
                    (float)((switcher_timer - THEME_SWITCHER_FADE_TIME -
                             THEME_SWITCHER_HOLD_TIME) /
                            THEME_SWITCHER_SWITCH_TIME);
        }
}

void theme_switcher_render(int width, int height) {
        int x = width - THEME_SWITCHER_ICON * 3 - BASIC_SPACING * 2 -
                CONTAINER_MARGIN - CONTAINER_MARGIN - BASIC_MARGIN -
                BORDER_WIDTH - BORDER_WIDTH;
        int y = height - THEME_SWITCHER_ICON - CONTAINER_MARGIN -
                CONTAINER_MARGIN - BASIC_MARGIN - BORDER_WIDTH - BORDER_WIDTH;
        int w = THEME_SWITCHER_ICON * 3 + BASIC_SPACING * 2 + CONTAINER_MARGIN +
                CONTAINER_MARGIN + BORDER_WIDTH + BORDER_WIDTH;
        int h = THEME_SWITCHER_ICON + CONTAINER_MARGIN + CONTAINER_MARGIN +
                BORDER_WIDTH + BORDER_WIDTH;
        int icon_x;
        int icon_y;
        int icon_w;
        int icon_h;
        int center_x = x + w / 2;
        double distance_from_center;
        float scale;

        basic_theme_t current = basic_get_theme();

        basic_image_id_t prev_theme_icon =
            basic_get_theme_icon(basic_theme_offset(current, -1));
        basic_image_id_t current_theme_icon = basic_get_theme_icon(current);
        basic_image_id_t next_theme_icon =
            basic_get_theme_icon(basic_theme_offset(current, 1));
        basic_image_id_t next_next_theme_icon =
            basic_get_theme_icon(basic_theme_offset(current, 2));

        int icon_x_offset = (int)(switcher_animation_progress *
                                  (THEME_SWITCHER_ICON + BASIC_SPACING));

        if (switcher_alpha <= 0.0f)
                return;

        basic_fill_rounded_rect(x, y, w, h, BORDER_ROUNDED, border_color);
        basic_fill_rounded_rect(
            x + BORDER_WIDTH, y + BORDER_WIDTH, w - BORDER_WIDTH - BORDER_WIDTH,
            h - BORDER_WIDTH - BORDER_WIDTH, BORDER_ROUNDED, page_background);

        /* we must also shrink icons basic on distance from the center position
         */
        icon_x = x + BORDER_WIDTH + CONTAINER_MARGIN + icon_x_offset;
        icon_y = y + BORDER_WIDTH + CONTAINER_MARGIN;
        icon_w = THEME_SWITCHER_ICON;
        icon_h = THEME_SWITCHER_ICON;
        distance_from_center = (icon_x + (double)icon_w / 2.0) - center_x;
        scale = 1.0f - (float)fabs(distance_from_center) / (float)(w / 2);
        icon_w = (int)(icon_w * scale);
        icon_h = (int)(icon_h * scale);
        /* center the icon based on new size */
        icon_x += (THEME_SWITCHER_ICON - icon_w) / 2;
        icon_y += (THEME_SWITCHER_ICON - icon_h) / 2;

        draw_image_scaled_cropped(prev_theme_icon, icon_x, icon_y, icon_w,
                                  icon_h, x + BORDER_WIDTH, y + BORDER_WIDTH,
                                  x + w - BORDER_WIDTH, y + h - BORDER_WIDTH);

        icon_x = x + BORDER_WIDTH + CONTAINER_MARGIN + THEME_SWITCHER_ICON +
                 BASIC_SPACING + icon_x_offset;
        icon_y = y + BORDER_WIDTH + CONTAINER_MARGIN;
        icon_w = THEME_SWITCHER_ICON;
        icon_h = THEME_SWITCHER_ICON;
        distance_from_center = (icon_x + (double)icon_w / 2.0) - center_x;
        scale = 1.0f - (float)fabs(distance_from_center) / (float)(w / 2);
        icon_w = (int)(icon_w * scale);
        icon_h = (int)(icon_h * scale);
        /* center the icon based on new size */
        icon_x += (THEME_SWITCHER_ICON - icon_w) / 2;
        icon_y += (THEME_SWITCHER_ICON - icon_h) / 2;

        draw_image_scaled(current_theme_icon, icon_x, icon_y, icon_w, icon_h);

        icon_x = x + BORDER_WIDTH + CONTAINER_MARGIN + THEME_SWITCHER_ICON * 2 +
                 BASIC_SPACING * 2 + icon_x_offset;
        icon_y = y + BORDER_WIDTH + CONTAINER_MARGIN;
        icon_w = THEME_SWITCHER_ICON;
        icon_h = THEME_SWITCHER_ICON;
        distance_from_center = (icon_x + (double)icon_w / 2.0) - center_x;
        scale = 1.0f - (float)fabs(distance_from_center) / (float)(w / 2);
        icon_w = (int)(icon_w * scale);
        icon_h = (int)(icon_h * scale);
        /* center the icon based on new size */
        icon_x += (THEME_SWITCHER_ICON - icon_w) / 2;
        icon_y += (THEME_SWITCHER_ICON - icon_h) / 2;

        draw_image_scaled_cropped(next_theme_icon, icon_x, icon_y, icon_w,
                                  icon_h, x + BORDER_WIDTH, y + BORDER_WIDTH,
                                  x + w - BORDER_WIDTH, y + h - BORDER_WIDTH);

        icon_x = x + BORDER_WIDTH + CONTAINER_MARGIN + THEME_SWITCHER_ICON * 3 +
                 BASIC_SPACING * 3 + icon_x_offset;
        icon_y = y + BORDER_WIDTH + CONTAINER_MARGIN;
        icon_w = THEME_SWITCHER_ICON;
        icon_h = THEME_SWITCHER_ICON;
        distance_from_center = (icon_x + (double)icon_w / 2.0) - center_x;
        scale = 1.0f - (float)fabs(distance_from_center) / (float)(w / 2);
        icon_w = (int)(icon_w * scale);
        icon_h = (int)(icon_h * scale);
        /* center the icon based on new size */
        icon_x += (THEME_SWITCHER_ICON - icon_w) / 2;
        icon_y += (THEME_SWITCHER_ICON - icon_h) / 2;

        draw_image_scaled_cropped(next_next_theme_icon, icon_x, icon_y, icon_w,
                                  icon_h, x + BORDER_WIDTH, y + BORDER_WIDTH,
                                  x + w - BORDER_WIDTH, y + h - BORDER_WIDTH);
}
