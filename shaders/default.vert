#version 330 core

layout (location = 0) in vec2 iPos;
layout (location = 1) in vec2 iTexCoord;

out vec2 oTexCoord;

uniform mat4 uModel;
uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * uModel * vec4(iPos, 0.0, 1.0);
    oTexCoord = iTexCoord;
}
