#include <assert.h>
#include "app.h"

Element make_sidebar(struct App *app) {
    Element el = {0};
    make_element(&el, SIDEBAR, 0, MENU_HEIGHT, SIDE_WIDTH,
                 app->height - MENU_HEIGHT);

    return el;
}

void update_sidebar(struct App *app, Element *el) {
    assert(el->type == SIDEBAR);
    SidebarData *data = el->data;

    el->height = app->height - MENU_HEIGHT;

    if (data) {
        if (data->to_destroy) {
            free(data);
            el->data = NULL;

            for (int i = 0; i < el->child_count; i++) {
                destroy_element(&el->children[i]);
            }

            el->child_count = 0;
        } else
            data->update(app, el);
    }

    for (int i = 0; i < el->child_count; i++) {
        update_element(app, &el->children[i]);
    }
}

void draw_sidebar(const struct App *app, const Element *el) {
    assert(el->type == SIDEBAR);

    set_color(CLR_MENUD);
    draw_rect(app, el->x, el->y, el->width, el->height);

    if (el->data) {
        ((SidebarData *) el->data)->draw(app);
    }

    for (int i = 0; i < el->child_count; i++) {
        draw_element(app, &el->children[i]);
    }
}

bool sidebar_on_click(struct App *app, Element *el, int x, int y, int button,
                      int action) {
    assert(el->type == SIDEBAR);
    for (int i = 0; i < el->child_count; i++)
        if (element_on_click(app, &el->children[i], x, y, button, action))
            return true;
    if (el->data) {
        SidebarData *data = el->data;
        if (data->on_click && data->on_click(app, el, x, y, button, action))
            return true;
    }
    return element_is_hovered(app, el);
}
