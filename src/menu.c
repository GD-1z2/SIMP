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

    int margin = (int) (app->scale * 20.f);

    el->width = 10;
    el->height = data->item_count * ITEM_HEIGHT;
    for (int i = 0; i < data->item_count; i++) {
        int w = (int) text_width(&app->font, items[i]) + margin;
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
        int id = (int) ((app->mouse_y - (float) el->y) / ITEM_HEIGHT);
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

    set_color(CLR_MENUL);
    draw_rect(app, el->x, el->y, el->width, el->height);

    if (data->selected_item != -1) {
        if (glfwGetMouseButton(app->window, GLFW_MOUSE_BUTTON_LEFT) ==
            GLFW_PRESS) {
            set_color(CLR_MENUD);
            draw_rect(app, el->x, el->y + ITEM_HEIGHT * data->selected_item,
                      el->width, ITEM_HEIGHT);
        } else {
            set_color(CLR_ACCENT);
            draw_rect(app, el->x, el->y + ITEM_HEIGHT * data->selected_item,
                      el->width, ITEM_HEIGHT);

            set_color(CLR_MENU);
            draw_rect(app, el->x + LINE_WIDTH,
                      el->y + ITEM_HEIGHT * data->selected_item + LINE_WIDTH,
                      el->width - LINE_WIDTH * 2, ITEM_HEIGHT - LINE_WIDTH * 2);
        }
    }

    set_color(CLR_TEXT);
    for (int i = 0; i < data->item_count; i++) {
        render_text(app, &app->font, data->items[i],
                    (float) el->x + app->scale * 8.f,
                    (float) el->y + ITEM_HEIGHT + (float) i * ITEM_HEIGHT -
                    6 * app->scale);
    }
}

bool menu_on_click(struct App *app, Element *el, int x, int y, int button,
                   int action) {
    assert(el->type == MENU);
    assert(el->data);
    MenuData *data = (MenuData *) el->data;

    bool willclose = app->mouse_y > MENU_HEIGHT;

    if (data->selected_item != -1 && action == GLFW_RELEASE) {
        if (data->callbacks[data->selected_item]) {
            data->callbacks[data->selected_item](app);
        }
        data->to_destroy = true;
    } else if (willclose && action == GLFW_RELEASE) {
        data->to_destroy = true;
    }

    return willclose;
}
