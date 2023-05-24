#pragma once

#include <cglm/vec4.h>
#include <glad/glad.h>
#include <stdbool.h>

typedef struct {
    int width;
    int height;
    int channels;
    unsigned int texture;
} RawImage;

bool load_raw_image(const char *filename, RawImage *raw_image);

typedef struct {
    bool valid;
    int width;
    int height;
    int channels;
    unsigned int framebuffer_front;
    unsigned int framebuffer_back;
    unsigned int texture_front;
    unsigned int texture_back;
} Image;

struct App;

bool create_empty_image(struct App *app, int width, int height, vec4 color,
                        Image *image);

bool create_image_from_raw(struct App *app, RawImage raw_image, Image *image);

void swap_image_buffers(Image *image);

void destroy_image(Image *image);
