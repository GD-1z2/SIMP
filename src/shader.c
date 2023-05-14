#include "shader.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bool
create_shader(GLenum shader_type, const char *file, GLuint *result) {
    FILE *fp = fopen(file, "r");
    if (!fp) {
        printf("Failed to open file %s\n", file);
        return false;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *source = malloc(size + 1);
    if (!source) {
        printf("Failed to allocate memory for shader source\n");
        return false;
    }
    fread(source, 1, size, fp);
    fclose(fp);
    source[size] = 0;

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, (const char **) &source, NULL);
    glCompileShader(shader);
    free(source);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success) {
        *result = shader;
        return true;
    }

    int length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    char infoLog[length];
    glGetShaderInfoLog(shader, length, NULL, infoLog);
    printf("Failed to compile shader_default:\n%s\n", infoLog);

    return false;
}

bool create_shader_program(const char *vert_name, const char *frag_name,
                           GLuint *result) {
    char vertFileName[256];
    char fragFileName[256];
    snprintf(vertFileName, 256, "shaders/%s.vert", vert_name);
    snprintf(fragFileName, 256, "shaders/%s.frag", frag_name);

    unsigned int vertexShader, fragmentShader, shaderProgram;
    if (!create_shader(GL_VERTEX_SHADER, vertFileName, &vertexShader) ||
        !create_shader(GL_FRAGMENT_SHADER, fragFileName, &fragmentShader)) {
        return false;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success) {
        *result = shaderProgram;
        return true;
    }

    int length;
    glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length);
    char infoLog[length];
    glGetProgramInfoLog(shaderProgram, length, NULL, infoLog);
    printf("Failed to link shader program:\n%s\n", infoLog);

    return false;
}