#include "app.h" /*
*/
#include <GLFW/glfw3.h>
#include <assert.h>
#include <string.h>

void input_init(Element *el, const char *text) {
    assert(el->type == INPUT);
    assert(text);

    InputData *data = malloc(sizeof(InputData));
    assert(data);

    *data = (InputData){
        .label = malloc(strlen(text) + 1),
        .hovered = false,
        .pressed = false,
    };

    assert(data->label);
    strcpy(data->label, text);

    el->data = data;

    if (el->height == 0) el->height = INPUT_H;
}

void update_input(struct App *app, Element *el) {
    assert(el->type == INPUT);

    InputData *data = el->data;
    assert(data);

    data->hovered = element_is_hovered(app, el);

    if (data->hovered && app->mouse_y > el->y + FONT_SIZE) {
        app->cursor = cursor_ibeam;
    }
}

void draw_input(const struct App *app, const Element *el) {
    assert(el->type == INPUT);

    const InputData *data = el->data;
    assert(data);

    int x = el->x;
    int y = el->y + FONT_SIZE;
    int width = el->width;
    int height = el->height - FONT_SIZE;

    if (el == app->focused) {
        set_color(CLR_ACCENT);
        draw_rect(app, x - LINE_WIDTH_TH, y - LINE_WIDTH_TH,
                  width + LINE_WIDTH_TH * 2, height + LINE_WIDTH_TH * 2);
    }

    set_color((vec4){.2f, .2f, .2f, 1.0f});
    render_text(app, &app->font, data->label, x, y - 5 * app->scale);

    if (data->hovered) {
        set_color((vec4){0.35f, 0.35f, 0.35f, 1.0f});
        draw_rect(app, x, y, width, height);
    } else {
        set_color((vec4){0.2f, 0.2f, 0.2f, 1.0f});
        draw_rect(app, x, y, width, height);
    }

    set_color(CLR_MENUL);
    render_text(app, &app->font, data->value, x + 5 * app->scale,
                y + 20 * app->scale);
}

bool on_click_input(struct App *app, Element *el, int x, int y, int button,
                    int action) {
    assert(el->type == INPUT);

    InputData *data = el->data;
    assert(data);

    if (!element_is_hovered(app, el)) {
        return false;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        data->pressed = true;
        return true;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        data->pressed = false;
        app->focused = el;
        return true;
    }

    return false;
}

void on_key_input(struct App *app, Element *el, int key, int scancode,
                  int action, int mods) {
    assert(el->type == INPUT);

    InputData *data = el->data;
    assert(data);

    if (el != app->focused) {
        return;
    }

    if (action != GLFW_RELEASE) {
        if (key == GLFW_KEY_BACKSPACE) {
            int len = strlen(data->value);
            if (len > 0) {
                data->value[len - 1] = '\0';
            }
        }
    }
}

void on_char_input(struct App *app, Element *el, unsigned int codepoint) {
    assert(el->type == INPUT);

    InputData *data = el->data;
    assert(data);

    if (el != app->focused) {
        return;
    }

    if (codepoint >= 32 && codepoint <= 126) {
        int len = strlen(data->value);
        if (len < 32) {
            data->value[len] = codepoint;
            data->value[len + 1] = '\0';
        }
    }
}
