#include "app.h"
#include "effects.h"

static void close(struct App *app, struct Element *button) {
    SidebarData *data = app->sidebar.data;
    data->to_destroy = true;
}

static void apply(struct App *app, struct Element *button) {
    glBindFramebuffer(GL_FRAMEBUFFER, app->image.framebuffer_back);
    glViewport(0, 0, app->image.width, app->image.height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(app->shader_colorize);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app->image.texture_front);

    glUniform1i(glGetUniformLocation(app->shader_colorize, "uTexture1"), 0);

    Element *set = app->sidebar.children;
    vec4 color = GLM_VEC4_ZERO_INIT;
    for (int i = 0; i < 4; ++i)
        color[i] = ((SliderData *) set->children[i].data)->value;
    glUniform4fv(glGetUniformLocation(app->shader_colorize, "uColor"), 1,
                 color);

    draw_rect(app, -1, -1, 2, 2);

    glUseProgram(app->shader_default);

    swap_image_buffers(&app->image);

    close(app, button);
}

static void update(struct App *app, Element *el) {

}

static void draw(const struct App *app) {
    set_color(CLR_TEXT);
    draw_rect(app, 0, MENU_HEIGHT + 13 * app->scale, SIDE_WIDTH, 1);

    set_color(CLR_MENUD);
    draw_rect(app, 10, MENU_HEIGHT,
              (int) text_width(&app->font, "Colorize") + 10 * app->scale,
              FONT_SIZE);

    set_color(CLR_TEXT);
    render_text(app, &app->font, "Colorize", 15, MENU_HEIGHT + 18 * app->scale);

    int row_h = 30.f * app->scale;
    int txt_x = 12 * app->scale;
    int txt_y = MENU_HEIGHT + FONT_SIZE + 30 * app->scale;

    set_color((vec4) {.5f, 0, 0, 1});
    render_text(app, &app->font, "R", txt_x, txt_y);
    set_color((vec4) {0, .5f, 0, 1});
    render_text(app, &app->font, "G", txt_x, txt_y + row_h);
    set_color((vec4) {0, 0, .5f, 1});
    render_text(app, &app->font, "B", txt_x, txt_y + row_h * 2);
    set_color(CLR_TEXT);
    render_text(app, &app->font, "A", txt_x, txt_y + row_h * 3);
}

static bool on_click(struct App *app, struct Element *el, int x, int y,
                     int button, int action) {
    return false;
}

void colorize_image(struct App *app) {
    SidebarData *data = malloc(sizeof(SidebarData));
    *data = (SidebarData) {
        .data = NULL,
        .update = update,
        .draw = draw,
        .on_click = on_click,
    };

    app->sidebar.data = data;

    Element *set = malloc(sizeof(Element));
    make_element(set, SET, 0, 0, 0, 0);

    set->children = malloc(sizeof(Element) * 6);
    Element *cancel_button = &set->children[4];
    Element *apply_button = &set->children[5];

    int row_h = 30.f * app->scale;
    int slider_y = MENU_HEIGHT + FONT_SIZE + 10 * app->scale;

    for (int i = 0; i < 4; i++) {
        Element *slider = &set->children[i];
        make_element(slider, SLIDER,
                     40.f * app->scale, slider_y,
                     180.f * app->scale, row_h);
        make_slider(slider, 1, 0, 2, 0);

        slider_y += row_h;
    }

    int button_y = slider_y + 10 * app->scale;
    int button_margin = 10 * app->scale;
    int button_w = (SIDE_WIDTH - 3 * button_margin) / 2;
    int button_h = 30 * app->scale;

    make_element(cancel_button, BUTTON, button_margin, button_y, button_w,
                 button_h);
    make_button(cancel_button, "Cancel", close);

    make_element(apply_button, BUTTON, button_margin * 2 + button_w, button_y,
                 button_w, button_h);
    make_button(apply_button, "Apply", apply);

    set->child_count = 6;

    app->sidebar.children = set;
    app->sidebar.child_count = 1;
}
