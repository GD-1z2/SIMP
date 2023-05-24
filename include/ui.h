#pragma once

#include <cglm/vec4.h>

#include "effects.h"

struct App;

extern const vec4 CLR_BG;
extern const vec4 CLR_MENU;
extern const vec4 CLR_MENUD;
extern const vec4 CLR_MENUL;
extern const vec4 CLR_SIDE;
extern const vec4 CLR_ACCENT;
extern const vec4 CLR_TEXT;

extern int FONT_SIZE, LINE_WIDTH, LINE_WIDTH_TH, MENU_HEIGHT, SIDE_WIDTH,
    ITEM_HEIGHT, BUTTON_H, INPUT_H, SLIDER_H;

typedef enum {
    REMOVED = 0,

    SET,
    MENUBAR,
    MENU,
    SIDEBAR,
    BUTTON,
    SLIDER,
    INPUT,
} ElementType;

typedef struct Element {
    ElementType type;
    int x, y, width, height;
    struct Element *children;
    int child_count;
    void *data;
} Element;

typedef void (*ElemCallback)(struct App *app, Element *el);

typedef struct {
    bool horizontal, fit;
} SetData;

typedef struct {
    char **items;
    AppFn *callbacks;
    int item_count;
    int selected_item;
    bool to_destroy;
} MenuData;

typedef struct {
    void *data;
    void (*update)(struct App *app, struct Element *el);
    void (*draw)(const struct App *app);
    bool (*on_click)(struct App *app, struct Element *el, int x, int y,
                     int button, int action);
} SidebarData;

typedef struct {
    const char *text;
    ElemCallback on_click;
    bool hovered, pressed;
} ButtonData;

#define SLIDER_TEXT_LEN 32

typedef struct {
    float value, min, max, step;
    bool hovered, pressed;
    char text[SLIDER_TEXT_LEN];
} SliderData;

#define MAX_INPUT_LEN 256

typedef struct {
    char *label;
    char value[MAX_INPUT_LEN];
    bool hovered, pressed;
} InputData;

void init_renderer(const struct App *app);
void update_projection(const struct App *app);

void set_texture(unsigned int texture);
void set_color(const vec4 color);
void set_gradient(const vec4 color_top, const vec4 color_bottom);

void draw_rect(const struct App *app, int x, int y, int width, int height);
void draw_rect_centered(const struct App *app, int x, int y, int width,
                        int height);
void draw_separator(const struct App *app, int x, int y, int width,
                    const char *text);

void element_init(Element *el, ElementType type, int x, int y, int width,
                  int height);
void update_element(struct App *app, Element *el);
void draw_element(const struct App *app, const Element *el);
bool element_on_click(struct App *app, Element *el, int x, int y, int button,
                      int action);
void element_on_key(struct App *app, Element *el, int key, int scancode,
                    int action, int mods);
void element_on_char(struct App *app, Element *el, unsigned int codepoint);
void destroy_element(Element *el);
bool element_is_hovered(const struct App *app, const Element *el);
Element *element_add_child(Element *el, ElementType type);

#define DECLARE_ELEMENT_FUNCTIONS(element)                                     \
    void update_##element(struct App *app, Element *el);                       \
    void draw_##element(const struct App *app, const Element *el);             \
    bool on_click_##element(struct App *app, Element *el, int x, int y,        \
                            int button, int action);                           \
    void on_key_##element(struct App *app, Element *el, int key, int scancode, \
                          int action, int mods);                               \
    void on_char_##element(struct App *app, Element *el,                       \
                           unsigned int codepoint);

DECLARE_ELEMENT_FUNCTIONS(set)
void set_init(Element *el, bool horizontal, bool fit);

DECLARE_ELEMENT_FUNCTIONS(menu_bar)
Element menu_bar_create(struct App *app);

DECLARE_ELEMENT_FUNCTIONS(menu)
void make_menu(struct App *app, Element *el, char **items, AppFn *callbacks);

DECLARE_ELEMENT_FUNCTIONS(sidebar)
Element sidebar_init(struct App *app);
void sidebar_rem_children(Element *el);

DECLARE_ELEMENT_FUNCTIONS(button)
void button_init(Element *el, const char *text, ElemCallback on_click);

DECLARE_ELEMENT_FUNCTIONS(slider)
void slider_init(Element *el, float value, float min, float max, float step,
                 const char *text);

DECLARE_ELEMENT_FUNCTIONS(input)
void input_init(Element *el, const char *text);
