#pragma once

#include "effects.h"

#include <cglm/vec4.h>

struct App;
struct Element;

extern const vec4 CLR_BG;
extern const vec4 CLR_MENU;
extern const vec4 CLR_MENUD;
extern const vec4 CLR_MENUL;
extern const vec4 CLR_ACCENT;
extern const vec4 CLR_TEXT;

extern int FONT_SIZE;
extern int LINE_WIDTH;
extern int LINE_WIDTH_TH;
extern int MENU_HEIGHT;
extern int SIDE_WIDTH;
extern int ITEM_HEIGHT;

typedef enum {
    REMOVED = 0,

    SET,
    MENUBAR,
    MENU,
    SIDEBAR,
    BUTTON,
    SLIDER,
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
    bool to_destroy;
} SidebarData;

typedef struct {
    const char *text;
    ElemCallback on_click;
    bool hovered;
    bool pressed;
} ButtonData;

typedef struct {
    float value;
    float min;
    float max;
    float step;
    bool hovered;
    bool pressed;
} SliderData;

void init_renderer(const struct App *app);
void update_projection(const struct App *app);

void set_texture(unsigned int texture);
void set_color(const vec4 color);
void set_gradient(const vec4 color_top, const vec4 color_bottom);

void draw_rect(const struct App *app, int x, int y, int width, int height);
void draw_rect_centered(const struct App *app, int x, int y, int width,
                        int height);

void make_element(Element *el, ElementType type, int x, int y, int width,
                  int height);
void update_element(struct App *app, Element *el);
void draw_element(const struct App *app, const Element *el);
bool element_on_click(struct App *app, Element *el, int x, int y, int button,
                      int action);
void destroy_element(Element *el);
bool element_is_hovered(const struct App *app, const Element *el);

void update_set(struct App *app, Element *el);
void draw_set(const struct App *app, const Element *el);
bool set_on_click(struct App *app, Element *el, int x, int y, int button,
                  int action);

Element make_menu_bar(struct App *app);
void update_menu_bar(struct App *app, Element *el);
void draw_menu_bar(const struct App *app, const Element *el);
bool bar_on_click(struct App *app, Element *el, int x, int y, int button,
                  int action);

void make_menu(struct App *app, Element *el, char **items, AppFn *callbacks);
void update_menu(struct App *app, Element *el);
void draw_menu(const struct App *app, const Element *el);
bool menu_on_click(struct App *app, Element *el, int x, int y, int button,
                   int action);

Element make_sidebar(struct App *app);
void update_sidebar(struct App *app, Element *el);
void draw_sidebar(const struct App *app, const Element *el);
bool sidebar_on_click(struct App *app, Element *el, int x, int y, int button,
                      int action);

void make_button(Element *el, const char *text, ElemCallback on_click);
void update_button(struct App *app, Element *button);
void draw_button(const struct App *app, const Element *el);
bool button_on_click(struct App *app, Element *button, int x, int y,
                     int button_, int action);

void make_slider(Element *el, float value, float min, float max, float step);
void update_slider(struct App *app, Element *el);
void draw_slider(const struct App *app, const Element *el);
bool slider_on_click(struct App *app, Element *el, int x, int y, int button,
                     int action);
