#include "app.h"
#include <assert.h>
#include <GLFW/glfw3.h>

void make_slider(Element *el, float value, float min, float max, float step) {
    SliderData *data = malloc(sizeof(SliderData));
    *data = (SliderData) {value, min, max, step};
    el->data = data;
}

void update_slider(struct App *app, Element *el) {
    assert(el->type == SLIDER);
    assert(el->data);
    SliderData *data = el->data;

    if (element_is_hovered(app, el)) {
        data->hovered = true;
        app->cursor = cursor_hand;
    } else {
        data->hovered = false;
    }

    if (data->pressed) {
        float x = app->mouse_x - el->x;
        float value = data->min + x / el->width * (data->max - data->min);
        if (data->step > 0)
            value = roundf(value / data->step) * data->step;
        if (value < data->min)
            value = data->min;
        if (value > data->max)
            value = data->max;
        data->value = value;
    }
}

void draw_slider(const struct App *app, const Element *el) {
    assert(el->type == SLIDER);
    assert(el->data);
    SliderData *data = el->data;

    int y = el->y + el->height / 2 - LINE_WIDTH / 2;

    set_color(CLR_TEXT);
    draw_rect(app, el->x, y, el->width, LINE_WIDTH);

    float x = el->x + (data->value - data->min) / (data->max - data->min) *
                      el->width;
    draw_rect(app, x - 4, y - 4, 9, 9);

    if (data->hovered) {
        if (data->pressed)
            set_color(CLR_ACCENT);
        else
            set_color(CLR_MENU);
    } else
        set_color(CLR_MENUD);
    draw_rect(app, x - 3, y - 3, 7, 7);
}

bool slider_on_click(struct App *app, Element *el, int x, int y, int button,
                     int action) {
    assert(el->type == SLIDER);
    assert(el->data);
    SliderData *data = el->data;

    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return false;

    if (action == GLFW_PRESS && data->hovered) {
        data->pressed = true;
        return true;
    } else if (action == GLFW_RELEASE && data->pressed) {
        data->pressed = false;
        return true;
    } else {
        return false;
    }
}
