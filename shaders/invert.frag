#version 330 core

out vec4 FragColor;

in vec2 oTexCoord;

uniform sampler2D texture1;

void main()
{
    vec4 color = texture(texture1, oTexCoord);
    FragColor = vec4(1.0 - texture(texture1, oTexCoord).rgb, color.a);
}
