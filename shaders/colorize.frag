#version 330 core

out vec4 FragColor;

in vec2 oTexCoord;

uniform sampler2D texture1;
uniform vec4 uColor;

void main()
{
    FragColor = texture(texture1, oTexCoord) * uColor;
}
