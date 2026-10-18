#include "render.h"
#include "design.h"
#include "images.h"
#include "theme.h"
#include "widgets/button.h"
#include "widgets/group.h"
#include "widgets/loading_bar.h"
#include "widgets/progress_bar.h"
#include "widgets/split.h"
#include "widgets/text.h"
#include "widgets/text_input.h"

#define BASIC_MARGIN 8
#define BASIC_SPACING 4

static int hitbox_count = 0;

void basic_render(struct ui_context *ctx) {
        int i;
        int height = BASIC_MARGIN;
        struct basic_render_details details;

        switch (basic_get_theme_type(basic_get_theme())) {
        case THEME_COLOR:
        default:
                basic_clear(page_background);
                break;
#ifdef ENABLE_THEMES
        case THEME_IMAGE:
                if (basic_get_theme() == THEME_GERMAN) {
                        /* The German theme is drawn scaled, not tiled */
                        draw_image_scaled(IMAGE_GERMAN_BACKGROUND, 0, 0, 800,
                                          600);

                } else
                        draw_image_tiled(BASIC_THEME_IMAGE(BACKGROUND), 0, 0,
                                         800, 600, 32, 32);
                break;
#endif
        }

        details.width =
            800 - BASIC_MARGIN - BASIC_MARGIN; /* TODO: Match window */
        details.x = BASIC_MARGIN;

        hitbox_count = 0;

        for (i = 0; i < ctx->count; i++) {
                details.y = height;
                height += basic_render_widget(&ctx->widgets[i], details) +
                          BASIC_SPACING;
        }
}

int basic_render_widget(struct widget *widget,
                        struct basic_render_details details) {
        switch (widget->type) {
        case WIDGET_TEXT:
                return basic_render_text_widget(*widget, details);
                break;
        case WIDGET_LOADING_BAR:
                return basic_render_loading_bar_widget(*widget, details);
                break;
        case WIDGET_PROGRESS_BAR:
                return basic_render_progress_bar_widget(*widget, details);
                break;
        case WIDGET_TEXT_INPUT: {
                int height = basic_render_text_input_widget(*widget, details);
                basic_register_hitbox(widget, details.x, details.y,
                                      details.width, height);
                return height;
                break;
        }
        case WIDGET_BUTTON: {
                int height = basic_render_button_widget(*widget, details);
                basic_register_hitbox(widget, details.x, details.y,
                                      details.width, height);
                return height;
                break;
        }
        case WIDGET_SPLIT:
                return basic_render_split_widget(*widget, details);
                break;
        case WIDGET_GROUP:
                return basic_render_group_widget(*widget, details);
                break;
        default:
                break;
        }

        return 0;
}

static struct basic_hitbox hitboxes[20]; /* todo make dynamic */

static void basic_register_hitbox(struct widget *widget, int x, int y,
                                  int width, int height) {
        int i;

        for (i = 0; i < hitbox_count; i++) {
                if (hitboxes[i].widget == widget) {
                        hitboxes[i].x = x;
                        hitboxes[i].y = y;
                        hitboxes[i].width = width;
                        hitboxes[i].height = height;
                        return;
                }
        }

        hitboxes[hitbox_count].widget = widget;
        hitboxes[hitbox_count].x = x;
        hitboxes[hitbox_count].y = y;
        hitboxes[hitbox_count].width = width;
        hitboxes[hitbox_count].height = height;
        hitbox_count++;
}

struct widget *basic_get_widget_at(int x, int y) {
        int i;
        for (i = 0; i < hitbox_count; i++) {
                if (x >= hitboxes[i].x &&
                    x < (hitboxes[i].x + hitboxes[i].width) &&
                    y >= hitboxes[i].y &&
                    y < (hitboxes[i].y + hitboxes[i].height))
                        return hitboxes[i].widget;
        }

        return NULL;
}
