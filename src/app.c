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

void scroll_callback(GLFWwindow *window, __attribute__((unused)) double x_off_d,
                     double y_off_d);

void run_app(struct App *app) {
    glfwSetWindowUserPointer(app->window, app);
    glfwSetMouseButtonCallback(app->window, mouse_button_callback);
    glfwSetScrollCallback(app->window, scroll_callback);

    cursor_hand = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    cursor_arrow = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

    app->scale =
        (float) glfwGetVideoMode(glfwGetPrimaryMonitor())->height / 1080.f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const float vertices[] = {.0f, .0f, 0.0f, 0.0f, 1.0f, .0f, 1.0f, 0.0f,
                              1.0f, 1.0f, 1.0f, 1.0f, .0f, 1.0f, 0.0f, 1.0f};
    const unsigned int indices[] = {0, 1, 2, 0, 2, 3};

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
    init_renderer(app);
    update_projection(app);

    if (!load_raw_image("test.png", &app->raw_image)
        || !create_image_from_raw(app, app->raw_image, &app->image)
        || !load_font(app, "Inter-Regular.ttf", &app->font)) {
        return;
    }

    app->menubar = make_menu_bar(app);
    app->sidebar = make_sidebar(app);

    while (!glfwWindowShouldClose(app->window)) {
        glfwPollEvents();

        // Update
        glfwGetWindowSize(app->window, &app->width, &app->height);
        double mouse_x_d, mouse_y_d;
        glfwGetCursorPos(app->window, &mouse_x_d, &mouse_y_d);
        app->mouse_x = (float) mouse_x_d;
        app->mouse_y = (float) mouse_y_d;

        app->cursor = cursor_arrow;

        if (app->dragging) {
            app->cursor = cursor_hand;

            float dx = app->mouse_x - app->x_drag_start;
            float dy = app->mouse_y - app->y_drag_start;
            app->x_drag_start = app->mouse_x;
            app->y_drag_start = app->mouse_y;
            app->x_offset += dx;
            app->y_offset += dy;
        }

        update_element(app, &app->menubar);
        update_element(app, &app->sidebar);

        // Render
        glUseProgram(app->shader_default);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, app->width, app->height);
        glClearColor(CLR_BG[0], CLR_BG[1], CLR_BG[2], CLR_BG[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        update_projection(app);

        float zoom = exp2f(app->zoom);
        set_texture(app->image.texture_front);
        draw_rect_centered(app, (app->width + SIDE_WIDTH) / 2.f + app->x_offset,
                           (app->height + MENU_HEIGHT) / 2.f + app->y_offset,
                           app->image.width * zoom, app->image.height * zoom);

        draw_element(app, &app->sidebar);
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
    float zoom_x = (float) (app->width - SIDE_WIDTH) / app->image.width;
    float zoom_y = (float) (app->height - MENU_HEIGHT) / app->image.height;
    app->zoom = log2f(glm_min(zoom_x, zoom_y));
}

void center_image(struct App *app) {
    app->x_offset = 0;
    app->y_offset = 0;
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
    struct App *app = glfwGetWindowUserPointer(window);

    if (!app->dragging &&
        element_on_click(app, &app->menubar, app->mouse_x, app->mouse_y, button,
                         action, mods))
        return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        app->x_drag_start = app->mouse_x;
        app->y_drag_start = app->mouse_y;
        app->dragging = true;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        app->x_drag_start = 0;
        app->y_drag_start = 0;
        app->dragging = false;
    }
}

void scroll_callback(GLFWwindow *window, __attribute__((unused)) double x_off_d,
                     double y_off_d) {
    struct App *app = glfwGetWindowUserPointer(window);
    float y_off = (float) y_off_d;
    app->zoom += y_off / 10;
    float min_zoom = log2f(100.f / (float) app->image.width);
    float max_zoom = log2f(10000.f / (float) app->image.width);

    if (app->zoom < min_zoom) {
        app->zoom = min_zoom;
    } else if (app->zoom > max_zoom) {
        app->zoom = max_zoom;
    } else {
        app->x_offset *= exp2f(y_off / 10);
        app->y_offset *= exp2f(y_off / 10);
    }
}
