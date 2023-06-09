#include <cglm/cam.h>

#include "app.h"
#include "effects.h"

static void close(struct App *app, struct Element *button) {
    sidebar_rem_children(&app->sidebar);
    app->live_fx = NULL;
}

static void live_fx(struct App *app) {
    glUseProgram(app->shader_colorize);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app->image.texture_front);

    glUniform1i(glGetUniformLocation(app->shader_colorize, "uTexture1"), 0);

    mat4 projection;
    glm_ortho(0.f, (float)app->width, (float)app->height, 0.f, -1.f, 1.f,
              projection);
    glUniformMatrix4fv(
        glGetUniformLocation(app->shader_colorize, "uProjection"), 1, GL_FALSE,
        (float *)projection);

    Element *set = app->sidebar.children;
    vec4 color = GLM_VEC4_ZERO_INIT;
    for (int i = 0; i < 4; ++i)
        color[i] = ((SliderData *)set->children[i].data)->value;
    glUniform4fv(glGetUniformLocation(app->shader_colorize, "uColor"), 1,
                 color);
}

static void apply(struct App *app, struct Element *button) {
    glBindFramebuffer(GL_FRAMEBUFFER, app->image.framebuffer_back);
    glViewport(0, 0, app->image.width, app->image.height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_BLEND);

    glUseProgram(app->shader_colorize);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app->image.texture_front);

    glUniform1i(glGetUniformLocation(app->shader_colorize, "uTexture1"), 0);

    mat4 projection;
    glm_ortho(0.0f, (float)app->image.width, 0.0f, (float)app->image.height,
              -1.0f, 1.0f, projection);
    glUniformMatrix4fv(
        glGetUniformLocation(app->shader_colorize, "uProjection"), 1, GL_FALSE,
        (float *)projection);

    Element *set = app->sidebar.children;
    vec4 color = GLM_VEC4_ZERO_INIT;
    for (int i = 0; i < 4; ++i)
        color[i] = ((SliderData *)set->children[i].data)->value;
    glUniform4fv(glGetUniformLocation(app->shader_colorize, "uColor"), 1,
                 color);

    draw_rect(app, 0, 0, app->image.width, app->image.height);

    glUseProgram(app->shader_ui);
    glEnable(GL_BLEND);

    swap_image_buffers(&app->image);

    close(app, button);
}

static void update(struct App *app, Element *el) {}

static void draw(const struct App *app) {
    draw_separator(app, 0, MENU_HEIGHT + 13 * app->scale, SIDE_WIDTH,
                   "Colorize");
}

static bool on_click(struct App *app, struct Element *el, int x, int y,
                     int button, int action) {
    return false;
}

void colorize_image(struct App *app) {
    if (!check_image(app)) return;
    cancel_effect(app);

    SidebarData *data = malloc(sizeof(SidebarData));
    *data = (SidebarData){
        .data = NULL,
        .update = update,
        .draw = draw,
        .on_click = on_click,
    };

    app->sidebar.data = data;
    app->live_fx = live_fx;

    Element *set = element_add_child(&app->sidebar, SET);
    element_init(set, SET, 0, MENU_HEIGHT, SIDE_WIDTH,
                 app->height - MENU_HEIGHT);

    static const char *labels[] = {"R", "G", "B", "A"};

    for (int i = 0; i < 4; i++)
        slider_init(element_add_child(set, SLIDER), 1, 0, 2, 0, labels[i]);

    Element *cancel_button = element_add_child(set, BUTTON);
    button_init(cancel_button, "Cancel", close);

    Element *apply_button = element_add_child(set, BUTTON);
    button_init(apply_button, "Apply", apply);
}
