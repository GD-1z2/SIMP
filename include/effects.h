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

void invert_image(struct App *app);
void colorize_image(struct App *app);
