#include "image.h"
#include "app.h"

#include <assert.h>
#include <cglm/cglm.h>
#include <stb/stb_image.h>

bool load_raw_image(const char *filename, RawImage *raw_image) {
    // Load the image using stb_image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data == NULL) {
        printf("Failed to load image \"%s\"\n", filename);
        printf("%s\n", stbi_failure_reason());
        return false;
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    *raw_image = (RawImage) {width, height, nrChannels, texture};
    return true;
}

static void make_framebuffer(unsigned int *framebuffer, unsigned int *texture,
                             int width, int height) {
    glGenFramebuffers(1, framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);

    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           *texture, 0);
}

bool create_empty_image(struct App *app, int width, int height, vec4 color,
                        Image *image) {
    unsigned int framebuffer_front, texture_front;
    make_framebuffer(&framebuffer_front, &texture_front, width, height);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Failed to create framebuffer_front\n");
        return false;
    }

    unsigned int framebuffer_back, texture_back;
    make_framebuffer(&framebuffer_back, &texture_back, width, height);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Failed to create framebuffer_back\n");
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_front);
    glViewport(0, 0, width, height);
    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT);

    *image = (Image) {
        true,
        width, height, 4,
        framebuffer_front, framebuffer_back,
        texture_front, texture_back};
    return true;
}

bool create_image_from_raw(struct App *app, RawImage raw_image, Image *image) {
    if (!create_empty_image(app, raw_image.width, raw_image.height,
                            (vec4) {0.0f, 0.0f, 0.0f, 0.0f}, image)) {
        return false;
    }

    glUseProgram(app->shader_ui);
    mat4 projection;
    glm_ortho(0.0f, (float) raw_image.width, (float) raw_image.height, 0.0f,
              -1.0f, 1.0f, projection);
    glUniformMatrix4fv(glGetUniformLocation(app->shader_ui, "uProjection"),
                       1, GL_FALSE, (float *) projection);

    set_texture(raw_image.texture);
    draw_rect(app, 0, 0, raw_image.width, raw_image.height);

    return true;
}

void swap_image_buffers(Image *image) {
    assert(image->valid);

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
    image->valid = false;
}
