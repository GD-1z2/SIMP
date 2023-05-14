#include "app.h" /*
*/
#include <GLFW/glfw3.h>

#include "assert.h"

#define MENU_COUNT 4

typedef struct {
    int i;
    const char *text;
    int x, width;
} BarButton;

typedef struct {
    int selected_item;
    int open_item;
    BarButton items[4];
} BarData;

static void close_menu(Element *el, BarData *data);

char *menu_bar_items[] = {"File", "Edit", "Effects", "View", NULL};

char *file_menu_items[] = {"New", "Open", "Save", NULL};
AppFn file_menu_callbacks[] = {NULL, NULL, NULL};

char *edit_menu_items[] = {"Invert colors", NULL};
AppFn edit_menu_callbacks[] = {invert_image};

char *effect_menu_items[] = {"Colorize", NULL};
AppFn effect_menu_callbacks[] = {colorize_image};

char *view_menu_items[] = {"Reset zoom", "Fit to screen", "Center image", NULL};
AppFn view_menu_callbacks[] = {reset_zoom, fit_to_window, center_image};

Element make_menu_bar(struct App *app) {
    Element el = {0};
    make_element(&el, MENUBAR, 0, 0, app->width, MENU_HEIGHT);

    BarData *data = calloc(1, sizeof(BarData));
    el.data = data;

    data->selected_item = -1;
    data->open_item = -1;

    int margin = (int) (app->scale * 30.f);

    int x = 0;
    for (int i = 0; i < MENU_COUNT; i++) {
        int width = (int) text_width(&app->font, menu_bar_items[i]) + margin;
        data->items[i] = (BarButton) {i, menu_bar_items[i], x, width};
        x += width;
    }

    return el;
}

void update_menu_bar(struct App *app, Element *el) {
    assert(el->type == MENUBAR);
    assert(el->data);
    BarData *data = el->data;

    el->width = app->width;

    data->selected_item = -1;

    if (el->children) {
        MenuData *menu_data = el->children->data;
        if (menu_data->to_destroy) {
            close_menu(el, data);
        } else {
            update_element(app, el->children);
        }
    }

    if (!element_is_hovered(app, el)) {
        return;
    }

    if (app->mouse_x >
        data->items[MENU_COUNT - 1].x + data->items[MENU_COUNT - 1].width) {
        return;
    }

    data->selected_item = 0;

    for (int i = 0; i < MENU_COUNT; i++) {
        if (app->mouse_x > data->items[i].x) {
            data->selected_item = i;
        }
    }

    app->cursor = cursor_hand;
}

static void draw_bar_button(const struct App *app, const BarData *data,
                            const BarButton *button) {
    if (button->i == data->open_item) {
        set_color(CLR_ACCENT);
        draw_rect(app, button->x, 0, button->width, MENU_HEIGHT);
    }

    if (button->i == data->selected_item) {
        if (glfwGetMouseButton(app->window, GLFW_MOUSE_BUTTON_LEFT) ==
            GLFW_PRESS) {
            set_color(CLR_MENUD);
            draw_rect(app, button->x, 0, button->width, MENU_HEIGHT);
        }
        set_color(CLR_ACCENT);
        draw_rect(app, button->x, MENU_HEIGHT - LINE_WIDTH_TH, button->width,
                  LINE_WIDTH_TH);
    }

    set_color(CLR_TEXT);
    float tw = text_width(&app->font, button->text);
    render_text(app, &app->font, button->text,
                button->x + button->width / 2 - tw / 2,
                (float) MENU_HEIGHT - 9 * app->scale);
}

void draw_menu_bar(const struct App *app, const Element *el) {
    assert(el->type == MENUBAR);
    assert(el->data);
    BarData *data = el->data;

    set_color(CLR_MENU);
    draw_rect(app, el->x, el->y, el->width, el->height);

    for (int i = 0; i < MENU_COUNT; i++)
        draw_bar_button(app, el->data, &data->items[i]);

    if (el->children) draw_element(app, el->children);
}

static void close_menu(Element *el, BarData *data) {
    assert(el->children);

    destroy_element(el->children);
    el->children = NULL;
    el->child_count = 0;
    data->open_item = -1;
}

static void
open_menu(struct App *app, Element *el, BarData *data, char **items,
          AppFn *callbacks) {
    assert(!el->children);

    Element *child = calloc(1, sizeof(Element));
    int x = data->items[data->selected_item].x;
    make_element(child, MENU, x, MENU_HEIGHT, 200, 200);
    make_menu(app, child, items, callbacks);

    el->children = child;
    el->child_count = 1;
    data->open_item = data->selected_item;
}

bool bar_on_click(struct App *app, Element *el, int x, int y, int button,
                  int action) {
    assert(el->type == MENUBAR);
    assert(el->data);
    BarData *data = el->data;

    if (app->mouse_y > MENU_HEIGHT) {
        return false;
    }

    if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_RELEASE ||
        data->selected_item == -1) {
        return true;
    }

    bool open = data->open_item != data->selected_item;

    if (data->open_item != -1)
        close_menu(el, data);

    if (!open) return true;

    switch (data->selected_item) {
        case 0:
            open_menu(app, el, data, file_menu_items, file_menu_callbacks);
            break;
        case 1:
            open_menu(app, el, data, edit_menu_items, edit_menu_callbacks);
            break;
        case 2:
            open_menu(app, el, data, effect_menu_items, effect_menu_callbacks);
            break;
        case 3:
            open_menu(app, el, data, view_menu_items, view_menu_callbacks);
            break;
    }

    return true;
}
