#include "font.h"

#include <glad/glad.h>
#include <stb/stb_truetype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"

bool load_font(const struct App *app, const char *filename, Font *result) {
    Font font = {0};

    // Load the font using stb_truetype
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Failed to open font file \"%s\"\n", filename);
        return false;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char *data = malloc(size);
    fread(data, 1, size, file);
    fclose(file);

    if (!stbtt_InitFont(&font.info, data, 0)) {
        printf("Failed to initialize font \"%s\"\n", filename);
        return false;
    }

    font.glyphs = malloc(sizeof(stbtt_bakedchar) * 96);

    // Create the font atlas
    int width = 512, height = 512;
    unsigned char *atlas = malloc(width * height);
    stbtt_BakeFontBitmap(data, 0, (float) FONT_SIZE, atlas, width, height, 32,
                         96, font.glyphs);

    // Create the font texture
    glGenTextures(1, &font.texture);
    glBindTexture(GL_TEXTURE_2D, font.texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
                 GL_UNSIGNED_BYTE, atlas);
    glGenerateMipmap(GL_TEXTURE_2D);

    free(atlas);
    free(data);

    *result = font;
    return true;
}

void render_text(const struct App *app, const Font *font, const char *text,
                 float x, float y) {
    glUseProgram(app->shader_ui);
    glUniform1i(glGetUniformLocation(app->shader_ui, "uTexture1"), 0);
    glUniform1i(glGetUniformLocation(app->shader_ui, "uText"), 1);
    glUniform1i(glGetUniformLocation(app->shader_ui, "uGradient"), 0);

    glBindVertexArray(app->vao);
    glBindBuffer(GL_ARRAY_BUFFER, app->vbo);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->texture);

    while (*text) {
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font->glyphs, 512, 512, *text - 32, &x, &y,
                               &q, 1);

            float vertices[] = {
                q.x0, q.y0, q.s0, q.t0,
                q.x1, q.y0, q.s1, q.t0,
                q.x1, q.y1, q.s1, q.t1,
                q.x0, q.y1, q.s0, q.t1,
            };

            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        ++text;
    }
}

float text_width(const Font *font, const char *text) {
    float width = 0.f;
    while (*text) {
        if (*text >= 32 && *text < 128) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font->glyphs, 512, 512, *text - 32, &width,
                               &(float) {0}, &q, 1);
        }

        ++text;
    }

    return width;
}

void destroy_font(Font *font) {
    glDeleteTextures(1, &font->texture);
    free(font->glyphs);
}
