#pragma once

#include <glad/glad.h>
#include <stdbool.h>

bool create_shader_program(const char *vert_name, const char *frag_name,
                           GLuint *result);
