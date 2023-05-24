#include "app.h" /*
*/

#include <GLFW/glfw3.h>
#include <assert.h>
#include <string.h>

void slider_init(Element *el, float value, float min, float max, float step,
                 const char *text) {
    SliderData *data = malloc(sizeof(SliderData));
    *data = (SliderData){value, min, max, step};
    strncpy(data->text, text, SLIDER_TEXT_LEN);
    el->data = data;

    if (el->height == 0) el->height = SLIDER_H;
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
        if (data->step > 0) value = roundf(value / data->step) * data->step;
        if (value < data->min) value = data->min;
        if (value > data->max) value = data->max;
        data->value = value;
    }
}

void draw_slider(const struct App *app, const Element *el) {
    assert(el->type == SLIDER);
    assert(el->data);
    SliderData *data = el->data;

    int y = el->y + 13 * app->scale;
    set_color(CLR_TEXT);
    render_text(app, &app->font, data->text, el->x, y);

    y = el->y + FONT_SIZE + 5 * app->scale;

    set_color(CLR_TEXT);
    draw_rect(app, el->x, y, el->width, LINE_WIDTH);

    int barx = el->x + 5 * app->scale;
    int barw = el->width - 10 * app->scale;

    float x = barx + (data->value - data->min) / (data->max - data->min) * barw;
    int btn_rad = 5 * app->scale, btn_width = 10 * app->scale + 1;
    draw_rect(app, x - btn_rad, y - btn_rad, btn_width, btn_width);

    if (data->hovered) {
        if (data->pressed)
            set_color(CLR_ACCENT);
        else
            set_color(CLR_MENU);
    } else
        set_color(CLR_MENUD);
    btn_rad = 5 * app->scale - 1, btn_width = 10 * app->scale - 1;
    draw_rect(app, x - btn_rad, y - btn_rad, btn_width, btn_width);
}

bool on_click_slider(struct App *app, Element *el, int x, int y, int button,
                     int action) {
    assert(el->type == SLIDER);
    assert(el->data);
    SliderData *data = el->data;

    if (button != GLFW_MOUSE_BUTTON_LEFT) return false;

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
