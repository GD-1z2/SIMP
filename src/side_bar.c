#include <assert.h>
#include "app.h"

Element make_sidebar(struct App *app) {
    Element el = {0};
    make_element(&el, SIDEBAR, 0, MENU_HEIGHT, SIDE_WIDTH,
                 app->height - MENU_HEIGHT);

    return el;
}

void update_sidebar(struct App *app, Element *el) {
    el->height = app->height - MENU_HEIGHT;
}

void draw_sidebar(const struct App *app, const Element *el) {
    assert(el->type == SIDEBAR);

    set_color(CLR_MENUD);
    draw_rect(app, el->x, el->y, el->width, el->height);
}
