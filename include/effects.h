#pragma once

typedef void (*AppFn)(struct App *);

typedef enum {
    EFFECT_INVERT,
    EFFECT_COLORIZE,

    EFFECT_COUNT
} Effect;

typedef enum {
    FXS_INVERT,
    FXS_COLORIZE,

    FXS_COUNT
} FxShader;

typedef struct {
    Effect current;
    AppFn effects[EFFECT_COUNT];
    unsigned int shaders[FXS_COUNT];
} EffectManager;

void cancel_effect(struct App *app);
bool check_image(const struct App *app);

void flip_image_h(struct App *app);
void flip_image_v(struct App *app);
void invert_image(struct App *app);
void scale_image(struct App *app);
void colorize_image(struct App *app);
void blur_image(struct App *app);
