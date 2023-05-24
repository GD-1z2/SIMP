#include "app.h"
#include "osdialog.h"

static void close(struct App *app, struct Element *button) {
    sidebar_rem_children(&app->sidebar);
    app->live_fx = NULL;
}

static void create_new(struct App *app, struct Element *button) {
    Element *width_input = &app->sidebar.children->children[0];
    Element *height_input = &app->sidebar.children->children[1];

    int width = atoi(((InputData *) width_input->data)->value);
    int height = atoi(((InputData *) height_input->data)->value);

    if (width <= 0 || height <= 0) {
        osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK,
                         "Width and height must be positive integers.");
        return;
    }

    destroy_image(&app->image);
    if (!create_empty_image(app, width, height, (vec4) {1, 1, 1, 1},
                            &app->image)) {
    }

    app->zoom = 1.f, app->x_offset = 0.f, app->y_offset = 0.f;

    close(app, button);
}

static void update(struct App *app, Element *el) {}

static void draw(const struct App *app) {
    set_color(CLR_TEXT);
    draw_rect(app, 0, MENU_HEIGHT + 13 * app->scale, SIDE_WIDTH, LINE_WIDTH);

    set_color(CLR_MENUD);
    draw_rect(app, 10, MENU_HEIGHT,
              (int) (text_width(&app->font, "New") + 10 * app->scale),
              FONT_SIZE);

    set_color(CLR_TEXT);
    render_text(app, &app->font, "New", 15, MENU_HEIGHT + 18 * app->scale);
}

static bool on_click(struct App *app, struct Element *el, int x, int y,
                     int button, int action) {
    return false;
}

void show_create_new(struct App *app) {
    cancel_effect(app);

    SidebarData *data = malloc(sizeof(SidebarData));
    *data = (SidebarData) {
        .data = NULL,
        .update = update,
        .draw = draw,
        .on_click = on_click,
    };

    app->sidebar.data = data;

    Element *set = element_add_child(&app->sidebar, SET);
    element_init(set, SET, 0, MENU_HEIGHT, SIDE_WIDTH,
                 app->height - MENU_HEIGHT);

    Element *width_input = element_add_child(set, INPUT);
    input_init(width_input, "Width");

    Element *height_input = element_add_child(set, INPUT);
    input_init(height_input, "Height");

    Element *cancel_button = element_add_child(set, BUTTON);
    button_init(cancel_button, "Cancel", close);

    Element *create_button = element_add_child(set, BUTTON);
    button_init(create_button, "Create", create_new);
}
