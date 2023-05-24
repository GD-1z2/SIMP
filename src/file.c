#include "app.h"
#include "osdialog.h"

void show_open(struct App *app) {
    const char *path = osdialog_file(OSDIALOG_OPEN, NULL, NULL, NULL);
    if (!path) return;
    load_image(app, path);
    app->zoom = 1.f, app->x_offset = 0.f, app->y_offset = 0.f;
    free((void *) path);
}

void show_save(struct App *app) {
    const char *path = osdialog_file(OSDIALOG_SAVE, NULL, NULL, NULL);
    if (path) {
        save_image(app, path);
        free((void *) path);
    }
}

void close_image(struct App *app) {
    destroy_image(&app->image);
}
