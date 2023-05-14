#include "app.h"
#include "effects.h"

void invert_image(struct App *app) {
    glBindFramebuffer(GL_FRAMEBUFFER, app->image.framebuffer_back);
    glViewport(0, 0, app->image.width, app->image.height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(app->shader_invert);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app->image.texture_front);

    glUniform1i(glGetUniformLocation(app->shader_invert, "uTexture1"), 0);

    draw_rect(app, -1, -1, 2, 2);

    glUseProgram(app->shader_default);

    swap_image_buffers(&app->image);
}

void darken_image(struct App *app) {
    glBindFramebuffer(GL_FRAMEBUFFER, app->image.framebuffer_back);
    glViewport(0, 0, app->image.width, app->image.height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(app->shader_colorize);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app->image.texture_front);

    glUniform1i(glGetUniformLocation(app->shader_colorize, "uTexture1"), 0);
    glUniform4f(glGetUniformLocation(app->shader_colorize, "uColor"), .9f, .9f,
                .9f, 1.f);

    draw_rect(app, -1, -1, 2, 2);

    glUseProgram(app->shader_default);

    swap_image_buffers(&app->image);
}
