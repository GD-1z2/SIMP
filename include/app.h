#pragma once

#include "font.h"
#include "image.h"
#include "ui.h"

struct GLFWwindow;

struct GLFWcursor;

extern struct GLFWcursor *cursor_hand;
extern struct GLFWcursor *cursor_arrow;

struct App {
    struct GLFWwindow *window;
    int width, height;
    unsigned int vao, vbo;
    unsigned int fs_vao, fs_vbo;
    unsigned int shader_default, shader_invert, shader_colorize;
    float mouse_x, mouse_y;
    RawImage raw_image;
    Image image;
    Font font;
    struct GLFWcursor *cursor;
    float zoom;
    Element menubar;
};

void run_app(struct App *app);

void reset_zoom(struct App *app);
void fit_to_window(struct App *app);
