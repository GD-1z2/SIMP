#include "app.h" /*
*/
#include <GLFW/glfw3.h>

#include "assert.h"
#include "cglm/cam.h"
#include "ui.h"

const vec4 CLR_BG = {0.28f, 0.3f, 0.3f, 1.0f};
const vec4 CLR_MENU = {0.9f, 0.9f, 0.9f, 1.0f};
const vec4 CLR_MENUD = {0.8f, 0.8f, 0.8f, 1.0f};
const vec4 CLR_MENUL = {0.95f, 0.95f, 0.95f, 1.0f};
const vec4 CLR_ACCENT = {.4f, .7f, 1.f, 1.f};
const vec4 CLR_TEXT = {0.1f, 0.1f, 0.1f, 1.0f};

int FONT_SIZE = 18;
int LINE_WIDTH = 1;
int LINE_WIDTH_TH = 2;
int MENU_HEIGHT = 30;
int SIDE_WIDTH = 250;
int ITEM_HEIGHT = 24;

static int SHD_PROJ, SHD_TEX1, SHD_TEXT, SHD_GRADIENT, SHD_CLR_TOP,
    SHD_CLR_BOTTOM;

void init_renderer(const struct App *app) {
    FONT_SIZE *= app->scale, LINE_WIDTH_TH *= app->scale,
    MENU_HEIGHT *= app->scale, SIDE_WIDTH *= app->scale,
    ITEM_HEIGHT *= app->scale;

    SHD_PROJ = glGetUniformLocation(app->shader_default, "uProjection");
    SHD_TEX1 = glGetUniformLocation(app->shader_default, "uTexture1");
    SHD_TEXT = glGetUniformLocation(app->shader_default, "uText");
    SHD_GRADIENT = glGetUniformLocation(app->shader_default, "uGradient");
    SHD_CLR_TOP = glGetUniformLocation(app->shader_default, "uColorTop");
    SHD_CLR_BOTTOM = glGetUniformLocation(app->shader_default, "uColorBottom");
}

void update_projection(const struct App *app) {
    mat4 projection;
    glm_ortho(0.f, (float) app->width, (float) app->height, 0.f, -1.f, 1.f,
              projection);
    glUniformMatrix4fv(SHD_PROJ, 1, GL_FALSE, (float *) projection);
}

void set_texture(unsigned int texture) {
    glUniform1i(SHD_TEXT, 0);
    glUniform1i(SHD_GRADIENT, 0);
    glUniform1i(SHD_TEX1, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void set_color(const vec4 color) {
    glUniform1i(SHD_TEXT, 0);
    glUniform1i(SHD_GRADIENT, 1);
    glUniform4f(SHD_CLR_TOP, color[0], color[1], color[2], color[3]);
    glUniform4f(SHD_CLR_BOTTOM, color[0], color[1], color[2], color[3]);
}

void set_gradient(const vec4 color_top, const vec4 color_bottom) {
    glUniform1i(SHD_TEXT, 0);
    glUniform1i(SHD_GRADIENT, 1);
    glUniform4f(SHD_CLR_TOP,
                color_top[0], color_top[1], color_top[2], color_top[3]);
    glUniform4f(SHD_CLR_BOTTOM,
                color_bottom[0], color_bottom[1], color_bottom[2],
                color_bottom[3]);
}

void draw_rect(const struct App *app, int x, int y, int width, int height) {
    glBindVertexArray(app->vao);
    glBindBuffer(GL_ARRAY_BUFFER, app->vbo);
    float vertices[] = {
        x, y, 0.f, 0.f,  //
        x + width, y, 1.f, 0.f,  //
        x + width, y + height, 1.f, 1.f,  //
        x, y + height, 0.f, 1.f,
    };
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
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
        case SET:
            update_set(app, el);
            return;

        case MENUBAR:
            update_menu_bar(app, el);
            return;

        case MENU:
            update_menu(app, el);
            return;

        case SIDEBAR:
            update_sidebar(app, el);
            break;

        case BUTTON:
            update_button(app, el);
            return;

        case SLIDER:
            update_slider(app, el);
            return;
    }
}

void draw_element(const struct App *app, const Element *el) {
    assert(el->type != REMOVED && "Element is removed");

    switch (el->type) {
        case SET:
            draw_set(app, el);
            return;

        case MENUBAR:
            draw_menu_bar(app, el);
            return;

        case MENU:
            draw_menu(app, el);
            return;

        case SIDEBAR:
            draw_sidebar(app, el);
            return;

        case BUTTON:
            draw_button(app, el);
            return;

        case SLIDER:
            draw_slider(app, el);
            return;
    }
}

bool element_on_click(struct App *app, Element *el, int x, int y, int button,
                      int action) {
    assert(el->type != REMOVED);

    for (int i = 0; i < el->child_count; i++) {
        if (element_on_click(app, &el->children[i], x, y, button, action)) {
            return true;
        }
    }

    switch (el->type) {
        case SET:
            return set_on_click(app, el, x, y, button, action);

        case MENUBAR:
            return bar_on_click(app, el, x, y, button, action);

        case MENU:
            return menu_on_click(app, el, x, y, button, action);

        case SIDEBAR:
            return sidebar_on_click(app, el, x, y, button, action);

        case BUTTON:
            return button_on_click(app, el, x, y, button, action);

        case SLIDER:
            return slider_on_click(app, el, x, y, button, action);
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

// Set impl

void update_set(struct App *app, Element *el) {
    assert(el->type == SET);

    for (int i = 0; i < el->child_count; i++) {
        update_element(app, &el->children[i]);
    }
}

void draw_set(const struct App *app, const Element *el) {
    assert(el->type == SET);

    for (int i = 0; i < el->child_count; i++) {
        draw_element(app, &el->children[i]);
    }
}

bool set_on_click(struct App *app, Element *el, int x, int y, int button,
                  int action) {
    assert(el->type == SET);

    for (int i = 0; i < el->child_count; i++) {
        if (element_on_click(app, &el->children[i], x, y, button, action)) {
            return true;
        }
    }

    return false;
}

// Button impl

void make_button(Element *button, const char *text, ElemCallback on_click) {
    assert(button->type == BUTTON);

    button->data = malloc(sizeof(ButtonData));
    ButtonData *data = (ButtonData *) button->data;

    data->text = text;
    data->on_click = on_click;
    data->hovered = false;
    data->pressed = false;
}

void update_button(struct App *app, Element *button) {
    assert(button->data);
    ButtonData *data = (ButtonData *) button->data;

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

void draw_button(const struct App *app, const Element *el) {
    assert(el->type == BUTTON);
    assert(el->data);
    ButtonData *data = (ButtonData *) el->data;

    set_color(CLR_TEXT);
    draw_rect(app, el->x, el->y, el->width, el->height);

    if (data->hovered) {
        if (data->pressed)
            set_color(CLR_ACCENT);
        else
            set_color(CLR_MENUL);
    } else
        set_color(CLR_MENU);
    draw_rect(app, el->x + LINE_WIDTH, el->y + LINE_WIDTH,
              el->width - LINE_WIDTH * 2, el->height - LINE_WIDTH * 2);

    set_color(CLR_TEXT);
    render_text(app, &app->font, data->text,
                el->x + (el->width - text_width(&app->font, data->text)) / 2,
                el->y + (el->height) / 2 + 5.5 * app->scale);
}

bool button_on_click(struct App *app, Element *button, int x, int y,
                     int button_, int action) {
    assert(button->data);
    ButtonData *data = (ButtonData *) button->data;

    if (button_ == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE &&
        data->hovered && data->on_click) {
        data->on_click(app, button);
        return true;
    }

    return false;
}
