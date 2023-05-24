#include "app.h"
#include "effects.h"
#include "osdialog.h"

#include <cglm/cam.h>
#include <cglm/affine.h>

static void transform_image(struct App *app, mat4 transform) {
    if (!check_image(app)) return;

    glBindFramebuffer(GL_FRAMEBUFFER, app->image.framebuffer_back);
    glViewport(0, 0, app->image.width, app->image.height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_BLEND);

    glUseProgram(app->shader_ui);
    set_texture(app->image.texture_front);

    glUniformMatrix4fv(glGetUniformLocation(app->shader_ui, "uProjection"),
                       1, GL_FALSE, (float *) transform);

    draw_rect(app, 0, 0, app->image.width, app->image.height);

    glEnable(GL_BLEND);

    swap_image_buffers(&app->image);
}

void flip_image_h(struct App *app) {
    mat4 transform;
    glm_ortho(-(float) app->image.width, 0.0f, 0.0f, (float) app->image.height,
              -1.0f, 1.0f, transform);
    glm_scale(transform, (vec3) {-1.f, 1.f, 1.f});
    transform_image(app, transform);
}

void flip_image_v(struct App *app) {
    mat4 transform;
    glm_ortho(0.0f, (float) app->image.width, -(float) app->image.height, 0.0f,
              -1.0f, 1.0f, transform);
    glm_scale(transform, (vec3) {1.f, -1.f, 1.f});
    transform_image(app, transform);
}

// Scale

static void close(struct App *app, struct Element *button) {
    sidebar_rem_children(&app->sidebar);
}

static void apply(struct App *app, struct Element *button) {
    struct Element *set = app->sidebar.children,
        *width_input = &set->children[0],
        *height_input = &set->children[1];

    int width = atoi(((InputData *) width_input->data)->value),
        height = atoi(((InputData *) height_input->data)->value);

    if (width <= 0 || height <= 0) {
        osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK,
                         "Width and height must be positive integers.");
        return;
    }

    Image new_image;
    if (!create_empty_image(app, width, height, (vec4) {0.f, 0.f, 0.f, 0.f},
                            &new_image)) {
        osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK,
                         "Failed to create new image.");
        return;
    }

    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, new_image.framebuffer_front);
    glViewport(0, 0, new_image.width, new_image.height);
    glUseProgram(app->shader_ui);
    set_texture(app->image.texture_front);
    mat4 projection;
    glm_ortho(0.0f, (float) new_image.width, 0.0f, (float) new_image.height,
              -1.0f, 1.0f, projection);
    glUniformMatrix4fv(glGetUniformLocation(app->shader_ui, "uProjection"), 1,
                       GL_FALSE, (float *) projection);
    draw_rect(app, 0, 0, new_image.width, new_image.height);

    glEnable(GL_BLEND);

    destroy_image(&app->image);
    app->image = new_image;

    close(app, button);
}

static void draw(const struct App *app) {
    draw_separator(app, 0, MENU_HEIGHT + 13 * app->scale, SIDE_WIDTH, "Scale");
}

void scale_image(struct App *app) {
    if (!check_image(app)) return;
    cancel_effect(app);

    SidebarData *data = malloc(sizeof(SidebarData));
    *data = (SidebarData) {
        .data = NULL,
        .update = NULL,
        .draw = draw,
        .on_click = NULL,
    };

    app->sidebar.data = data;

    Element *set = element_add_child(&app->sidebar, SET);
    element_init(set, SET, 0, MENU_HEIGHT, SIDE_WIDTH,
                 app->height - MENU_HEIGHT);

    input_init(element_add_child(set, INPUT), "Width");
    input_init(element_add_child(set, INPUT), "Height");

    Element *cancel_button = element_add_child(set, BUTTON);
    button_init(cancel_button, "Cancel", close);

    Element *apply_button = element_add_child(set, BUTTON);
    button_init(apply_button, "Apply", apply);
}
