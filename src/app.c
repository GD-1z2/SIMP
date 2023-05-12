#include "app.h"

#include <GLFW/glfw3.h>
#include <assert.h>
#include <cglm/cglm.h>
#include <stdio.h>

#include "font.h"
#include "image.h"
#include "math.h"
#include "shader.h"
#include "ui.h"

struct GLFWcursor *cursor_hand;
struct GLFWcursor *cursor_arrow;

static void mouse_button_callback(GLFWwindow *window, int button, int action,
                                  int mods);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void run_app(struct App *app) {
    glfwSetWindowUserPointer(app->window, app);
    glfwSetMouseButtonCallback(app->window, mouse_button_callback);
    glfwSetScrollCallback(app->window, scroll_callback);

    cursor_hand = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    cursor_arrow = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set up vertex data and buffers
    float vertices[] = {// positions          // raw_image coords
        .0f, .0f, 0.0f, 0.0f, 1.0f, .0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, .0f, 1.0f, 0.0f, 1.0f};

    unsigned int indices[] = {0, 1, 2,   // first triangle
                              0, 2, 3};  // second triangle

    unsigned int ebo;

    glGenVertexArrays(1, &app->vao);
    glGenBuffers(1, &app->vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(app->vao);
    glBindBuffer(GL_ARRAY_BUFFER, app->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int fs_ebo;
    float fs_vertices[] = {.0f, .0f, 0.0f, 0.0f,  //
                           1.0f, .0f, 1.0f, 0.0f,  //
                           1.0f, 1.0f, 1.0f, 1.0f,  //
                           .0f, 1.0f, 0.0f, 1.0f};

    glGenVertexArrays(1, &app->fs_vao);
    glGenBuffers(1, &app->fs_vbo);
    glGenBuffers(1, &fs_ebo);

    glBindVertexArray(app->fs_vao);
    glBindBuffer(GL_ARRAY_BUFFER, app->fs_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fs_ebo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(fs_vertices), fs_vertices,
                 GL_STATIC_DRAW);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *) (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    if (!load_raw_image("test.png", &app->raw_image)) {
        return;
    }
    assert(app->raw_image.width > 0 && "Failed to load raw_image");
    if (!create_shader_program("default", "default", &app->shader_default)) {
        printf("Failed to create shader_default program\n");
        return;
    }
    if (!create_shader_program("default_effect", "invert",
                               &app->shader_invert)) {
        printf("Failed to create shader_invert program\n");
        return;
    }
    if (!create_shader_program("default_effect", "colorize",
                               &app->shader_colorize)) {
        printf("Failed to create shader_colorize program\n");
        return;
    }
    glUseProgram(app->shader_default);
    int proj_loc = glGetUniformLocation(app->shader_default, "uProjection");
    int mod_loc = glGetUniformLocation(app->shader_default, "uModel");

    app->image = create_image_from_raw(app, app->raw_image);

    if (!load_font(app, "Inter-Regular.ttf", &app->font)) {
        printf("Failed to load font\n");
        return;
    }

    app->menubar = make_menu_bar(app);

    while (!glfwWindowShouldClose(app->window)) {
        glfwPollEvents();

        // Update
        glfwGetWindowSize(app->window, &app->width, &app->height);

        double mouse_x, mouse_y;
        glfwGetCursorPos(app->window, &mouse_x, &mouse_y);
        app->mouse_x = mouse_x;
        app->mouse_y = mouse_y;

        app->cursor = cursor_arrow;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, app->width, app->height);
        glClearColor(0.25f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        update_element(app, &app->menubar);

        // Render
        glUseProgram(app->shader_default);

        mat4 projection = GLM_MAT4_IDENTITY_INIT;
        glm_ortho(0.f, (float) app->width, (float) app->height, 0.f, -1.f, 1.f,
                  projection);
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (float *) projection);

        mat4 model = GLM_MAT4_IDENTITY_INIT;
        glUniformMatrix4fv(mod_loc, 1, GL_FALSE, (float *) model);

        float zoom = exp2f(app->zoom);
        set_texture(app, app->image.texture_front);
        draw_rect_centered(app, app->width / 2.f, app->height / 2.f + 24,
                           app->image.width * zoom, app->image.height * zoom);

        draw_element(app, &app->menubar);

        glfwSetCursor(app->window, app->cursor);

        glfwSwapBuffers(app->window);
    }

    destroy_image(&app->image);
    destroy_element(&app->menubar);
}

void reset_zoom(struct App *app) {
    app->zoom = 0;
}

void fit_to_window(struct App *app) {
    float zoom_x = (float) app->width / app->image.width;
    float zoom_y = (float) (app->height - 48) / app->image.height;
    app->zoom = log2f(glm_min(zoom_x, zoom_y));
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
    struct App *app = glfwGetWindowUserPointer(window);

    element_on_click(app, &app->menubar, app->mouse_x, app->mouse_y, button,
                     action, mods);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    struct App *app = glfwGetWindowUserPointer(window);
    app->zoom += yoffset / 10;
    app->zoom = glm_max(app->zoom, log2f(100.f / app->image.width));
    app->zoom = glm_min(app->zoom, log2f(10000.f / app->image.width));
}
