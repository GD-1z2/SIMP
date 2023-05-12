#include "image.h"
#include "app.h"

#include <cglm/cglm.h>
#include <stb/stb_image.h>

bool load_raw_image(const char *filename, RawImage *raw_image) {
    // Load the image using stb_image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data == NULL) {
        printf("Failed to load image \"%s\"\n", filename);
        return false;
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    *raw_image = (RawImage) {width, height, nrChannels, texture};
    return true;
}

Image create_image_from_raw(struct App *app, RawImage raw_image) {
    unsigned int framebuffer_front;
    glGenFramebuffers(1, &framebuffer_front);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_front);

    unsigned int texture_front;
    glGenTextures(1, &texture_front);
    glBindTexture(GL_TEXTURE_2D, texture_front);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, raw_image.width, raw_image.height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture_front, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Failed to create framebuffer_front\n");
        return (Image) {0, 0, 0, 0, 0, 0, 0};
    }

    unsigned int framebuffer_back;
    glGenFramebuffers(1, &framebuffer_back);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_back);

    unsigned int texture_back;
    glGenTextures(1, &texture_back);
    glBindTexture(GL_TEXTURE_2D, texture_back);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, raw_image.width, raw_image.height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture_back, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Failed to create framebuffer_front\n");
        return (Image) {0, 0, 0, 0, 0, 0, 0};
    }


    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_front);
    glViewport(0, 0, raw_image.width, raw_image.height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    mat4 projection;
    glm_ortho(0.0f, (float) raw_image.width, (float) raw_image.height, 0.0f, -1.0f, 1.0f,
              projection);
    glUseProgram(app->shader_default);
    glUniformMatrix4fv(glGetUniformLocation(app->shader_default, "uProjection"),
                       1, GL_FALSE, (float *) projection);

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3) {0.0f, 0.0f, 0.0f});
    glm_scale(model, (vec3) {raw_image.width, raw_image.height, 1.0f});
    glUniformMatrix4fv(glGetUniformLocation(app->shader_default, "uModel"),
                       1, GL_FALSE, (float *) model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, raw_image.texture);
    glBindVertexArray(app->vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


    return (Image) {
        raw_image.width, raw_image.height, raw_image.channels,
        framebuffer_front, framebuffer_back,
        texture_front, texture_back};
}

void swap_image_buffers(Image *image) {
    unsigned int framebuffer = image->framebuffer_front;
    image->framebuffer_front = image->framebuffer_back;
    image->framebuffer_back = framebuffer;

    unsigned int texture = image->texture_front;
    image->texture_front = image->texture_back;
    image->texture_back = texture;
}

void destroy_image(Image *image) {
    glDeleteFramebuffers(1, &image->framebuffer_front);
    glDeleteFramebuffers(1, &image->framebuffer_back);
    glDeleteTextures(1, &image->texture_front);
    glDeleteTextures(1, &image->texture_back);
}
