#include "app.h"
#include "effects.h"
#include <cglm/mat4.h>
#include <cglm/cam.h>

void invert_image(struct App *app) {
    glBindFramebuffer(GL_FRAMEBUFFER, app->image.framebuffer_back);
    glViewport(0, 0, app->image.width, app->image.height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(app->shader_invert);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app->image.texture_front);

    glUniform1i(glGetUniformLocation(app->shader_invert, "uTexture1"), 0);

    mat4 projection;
    glm_ortho(0.0f, (float) app->image.width, 0.0f, (float) app->image.height,
              -1.0f, 1.0f, projection);
    glUniformMatrix4fv(glGetUniformLocation(app->shader_invert, "uProjection"),
                       1, GL_FALSE, (float *) projection);

    draw_rect(app, 0, 0, app->image.width, app->image.height);

    glUseProgram(app->shader_default);

    swap_image_buffers(&app->image);
}
