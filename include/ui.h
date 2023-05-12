#pragma once

#include "effects.h"

#include <cglm/vec4.h>

struct App;
struct Element;

typedef enum {
    REMOVED = 0,

    BAR,
    MENU,
    BUTTON,
} ElementType;

typedef struct Element {
    ElementType type;

    int x;
    int y;
    int width;
    int height;

    struct Element *children;
    int child_count;

    void *data;
} Element;

typedef void (*ElemCallback)(struct App *app, Element *el);

typedef struct {
    const char *text;
    ElemCallback on_click;
    bool hovered;
    bool pressed;
} ButtonData;

typedef struct {
    char **items;
    AppFn *callbacks;
    int item_count;
    int selected_item;
    bool to_destroy;
} MenuData;

void set_texture(const struct App *app, unsigned int texture);
void set_color(const struct App *app, vec4 color);
void set_gradient(const struct App *app, vec4 color_top, vec4 color_bottom);

void draw_rect(const struct App *app, int x, int y, int width, int height);
void draw_rect_centered(const struct App *app, int x, int y, int width,
                        int height);

void make_element(Element *el, ElementType type, int x, int y, int width,
                  int height);
void update_element(struct App *app, Element *el);
void draw_element(const struct App *app, const Element *el);
bool element_on_click(struct App *app, Element *el, int x, int y, int button,
                      int action, int mods);
void destroy_element(Element *el);
bool element_is_hovered(const struct App *app, const Element *el);

void make_button(Element *el, const char *text, ElemCallback on_click);
void update_button(struct App *app, Element *button);
bool button_on_click(struct App *app, Element *button, int x, int y,
                     int button_, int action);

Element make_menu_bar(struct App *app);
void update_menu_bar(struct App *app, Element *el);
void draw_menu_bar(const struct App *app, const Element *el);
bool bar_on_click(struct App *app, Element *el, int x, int y, int button,
                  int action);

void
make_menu(struct App *app, Element *el, char **items, AppFn *callbacks);
void update_menu(struct App *app, Element *el);
void draw_menu(const struct App *app, const Element *el);
bool menu_on_click(struct App *app, Element *el, int x, int y, int button,
                   int action);
