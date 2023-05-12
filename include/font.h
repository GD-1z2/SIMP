#pragma once

#include <stb/stb_truetype.h>
#include <stdbool.h>

struct App;

typedef struct {
    stbtt_fontinfo info;
    stbtt_bakedchar *glyphs;
    unsigned int texture;
    unsigned int vao;
    unsigned int vbo;
} Font;

bool load_font(const struct App *app, const char *filename, Font *result);

void render_text(const struct App *app, const Font *font, const char *text,
                 float x, float y);

float text_width(const Font *font, const char *text);
