#version 330 core

out vec4 FragColor;

in vec2 oTexCoord;

uniform sampler2D uTexture1;
uniform bool uText;
uniform bool uGradient;
uniform vec4 uColorTop;
uniform vec4 uColorBottom;

void main()
{
    if (uGradient) {
        FragColor = mix(uColorBottom, uColorTop, oTexCoord.y);
        return;
    }
    if (uText) {
        FragColor = mix(uColorBottom, uColorTop, oTexCoord.y);
        FragColor.a = texture(uTexture1, oTexCoord).r;
        return;
    }

    FragColor = texture(uTexture1, oTexCoord);
}
