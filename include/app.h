#pragma once

#include "font.h"
#include "image.h"
#include "ui.h"

struct GLFWwindow;

struct GLFWcursor;

extern struct GLFWcursor *cursor_hand;
extern struct GLFWcursor *cursor_arrow;
extern struct GLFWcursor *cursor_ibeam;

struct App {
    struct GLFWwindow *window;
    float scale;
    int width, height;
    float mouse_x, mouse_y;
    bool dragging;
    unsigned int vao, vbo;
    unsigned int shader_ui, shader_invert, shader_colorize, shader_blur;
    Image image;
    Font font;
    struct GLFWcursor *cursor;
    float zoom, x_offset, y_offset, x_drag_start, y_drag_start;
    Element menubar, sidebar;
    Element *focused;
    EffectManager effects;
    AppFn live_fx;
};

void run_app(struct App *app);

bool load_image(struct App *app, const char *path);
bool save_image(struct App *app, const char *path);

void show_create_new(struct App *app);
void show_open(struct App *app);
void show_save(struct App *app);
void close_image(struct App *app);

void reset_zoom(struct App *app);
void fit_to_window(struct App *app);
void center_image(struct App *app);
