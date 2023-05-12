#include "app.h" /*
*/
#include <GLFW/glfw3.h>

#include "assert.h"
#include "ui.h"

void set_texture(const struct App *app, unsigned int texture) {
    glUniform1i(glGetUniformLocation(app->shader_default, "uText"), 0);
    glUniform1i(glGetUniformLocation(app->shader_default, "uGradient"), 0);

    glUniform1i(glGetUniformLocation(app->shader_default, "texture1"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void set_color(const struct App *app, vec4 color) {
    glUniform1i(glGetUniformLocation(app->shader_default, "uText"), 0);
    glUniform1i(glGetUniformLocation(app->shader_default, "uGradient"), 1);

    glUniform4f(glGetUniformLocation(app->shader_default, "uColorTop"),
                color[0], color[1], color[2], color[3]);
    glUniform4f(glGetUniformLocation(app->shader_default, "uColorBottom"),
                color[0], color[1], color[2], color[3]);
}

void set_gradient(const struct App *app, vec4 color_top, vec4 color_bottom) {
    glUniform1i(glGetUniformLocation(app->shader_default, "uText"), 0);
    glUniform1i(glGetUniformLocation(app->shader_default, "uGradient"), 1);

    glUniform4f(glGetUniformLocation(app->shader_default, "uColorTop"),
                color_top[0], color_top[1], color_top[2], color_top[3]);
    glUniform4f(glGetUniformLocation(app->shader_default, "uColorBottom"),
                color_bottom[0], color_bottom[1], color_bottom[2],
                color_bottom[3]);
}

void draw_rect(const struct App *app, int x, int y, int width, int height) {
    glBindVertexArray(app->vao);
    glBindBuffer(GL_ARRAY_BUFFER, app->vbo);

    float vertices[] = {
        x,         y,          0.f, 0.f,  //
        x + width, y,          1.f, 0.f,  //
        x + width, y + height, 1.f, 1.f,  //
        x,         y + height, 0.f, 1.f,
    };

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

//    glUseProgram(app->shader_default);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void draw_rect_centered(const struct App *app, int x, int y, int width,
                        int height) {
    draw_rect(app, x - width / 2, y - height / 2, width, height);
}

void make_element(Element *el, ElementType type, int x, int y, int width,
                  int height) {
    el->type = type;
    el->x = x;
    el->y = y;
    el->width = width;
    el->height = height;
    el->children = NULL;
    el->child_count = 0;
    el->data = NULL;
}

void update_element(struct App *app, Element *el) {
    assert(el->type != REMOVED);

    switch (el->type) {
        case BAR:
            update_menu_bar(app, el);
            return;

        case MENU:
            update_menu(app, el);
            return;

        case BUTTON:
            update_button(app, el);
            return;
    }
}

void draw_element(const struct App *app, const Element *el) {
    assert(el->type != REMOVED && "Element is removed");

    switch (el->type) {
        case BAR:
            draw_menu_bar(app, el);
            return;

        case MENU:
            draw_menu(app, el);
            return;

        case BUTTON:
            return;
    }
}

bool element_on_click(struct App *app, Element *el, int x, int y, int button,
                      int action, int mods) {
    assert(el->type != REMOVED);

    for (int i = 0; i < el->child_count; i++) {
        if (element_on_click(app, &el->children[i], x, y, button, action,
                             mods)) {
            return true;
        }
    }

    switch (el->type) {
        case BAR:
            return bar_on_click(app, el, x, y, button, action);

        case MENU:
            return menu_on_click(app, el, x, y, button, action);

        case BUTTON:
            return button_on_click(app, el, x, y, button, action);
    }

    return false;
}

void destroy_element(Element *el) {
    if (el->data) free(el->data);

    for (int i = 0; i < el->child_count; i++) destroy_element(&el->children[i]);

    if (el->children) free(el->children);

    el->type = REMOVED;
}

bool element_is_hovered(const struct App *app, const Element *el) {
    return app->mouse_x > el->x && app->mouse_x <= el->x + el->width &&
           app->mouse_y > el->y && app->mouse_y <= el->y + el->height;
}

void make_button(Element *button, const char *text, ElemCallback on_click) {
    assert(button->type == BUTTON);

    button->data = malloc(sizeof(ButtonData));
    ButtonData *data = (ButtonData *)button->data;

    data->text = text;
    data->on_click = on_click;
    data->hovered = false;
    data->pressed = false;
}

// Per-type impl

void update_button(struct App *app, Element *button) {
    assert(button->data);
    ButtonData *data = (ButtonData *)button->data;

    if (element_is_hovered(app, button)) {
        data->hovered = true;
        app->cursor = cursor_hand;
    } else {
        data->hovered = false;
    }

    data->pressed = data->hovered &&
                    glfwGetMouseButton(app->window, GLFW_MOUSE_BUTTON_LEFT);

    for (int i = 0; i < button->child_count; i++) {
        update_element(app, &button->children[i]);
    }
}

bool button_on_click(struct App *app, Element *button, int x, int y,
                     int button_, int action) {
    assert(button->data);
    ButtonData *data = (ButtonData *)button->data;

    if (button_ == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS &&
        data->hovered && data->on_click) {
        data->on_click(app, button);
        return true;
    }

    return false;
}
