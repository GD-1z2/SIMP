#pragma once

typedef void (*AppFn)(struct App *);

void invert_image(struct App *app);
void darken_image(struct App *app);
