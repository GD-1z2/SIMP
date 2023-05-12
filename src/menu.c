#include "app.h"
#include "assert.h"
#include "ui.h"

#include <GLFW/glfw3.h>

void make_menu(struct App *app, Element *el, char **items, AppFn *callbacks) {
    el->type = MENU;
    el->data = calloc(1, sizeof(MenuData));
    MenuData *data = (MenuData *) el->data;

    data->items = items;
    data->callbacks = callbacks;
    data->item_count = 0;

    for (int i = 0; items[i]; i++) data->item_count++;

    el->width = 10;
    el->height = data->item_count * 48;
    for (int i = 0; i < data->item_count; i++) {
        int w = text_width(&app->font, items[i]) + 20;
        if (w > el->width) el->width = w;
    }
}

void update_menu(struct App *app, Element *el) {
    assert(el->type == MENU);
    assert(el->data);
    MenuData *data = (MenuData *) el->data;

    if (element_is_hovered(app, el))
        app->cursor = cursor_hand;

    if (element_is_hovered(app, el)) {
        int id = (int) ((app->mouse_y - el->y) / 48);
        id = id < 0 ? 0 : id;
        id = id >= data->item_count ? data->item_count - 1 : id;
        data->selected_item = id;
    } else {
        data->selected_item = -1;
    }
}

void draw_menu(const struct App *app, const Element *el) {
    assert(el->type == MENU);
    assert(el->data);
    MenuData *data = (MenuData *) el->data;

    set_color(app, (vec4) {.8f, .8f, .8f, 1.f});
    draw_rect(app, el->x, el->y, el->width, el->height);

    if (data->selected_item != -1) {
        if (glfwGetMouseButton(app->window, GLFW_MOUSE_BUTTON_LEFT) ==
            GLFW_PRESS) {
            set_color(app, (vec4) {.9f, .9f, .9f, 1.f});
            draw_rect(app, el->x, el->y + 48 * data->selected_item,
                      el->width, 48);
        } else {
            set_color(app, (vec4) {.4f, .7f, 1.f, 1.f});
            draw_rect(app, el->x, el->y + 48 * data->selected_item,
                      el->width, 48);

            set_color(app, (vec4) {.8f, .8f, .8f, 1.f});
            draw_rect(app, el->x + 3, el->y + 48 * data->selected_item + 3,
                      el->width - 6, 48 - 6);
        }
    }

    set_color(app, (vec4) {.1f, .1f, .1f, 1.f});
    for (int i = 0; i < data->item_count; i++) {
        render_text(app, &app->font, data->items[i], el->x + 10,
                    el->y + 48 + i * 48 - 13);
    }
}

bool menu_on_click(struct App *app, Element *el, int x, int y, int button,
                   int action) {
    assert(el->type == MENU);
    assert(el->data);
    MenuData *data = (MenuData *) el->data;

    if (data->selected_item != -1 && action == GLFW_RELEASE) {
        if (data->callbacks[data->selected_item]) {
            data->callbacks[data->selected_item](app);
        }
        data->to_destroy = true;
        return true;
    } else if (app->mouse_y > 48 && action == GLFW_RELEASE) {
        data->to_destroy = true;
        return true;
    }

    return false;
}
