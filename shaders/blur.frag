// Blur filter
#version 330 core

out vec4 FragColor;

in vec2 oTexCoord;

uniform sampler2D texture1;
uniform int uSize;

void main()
{
    vec2 tex_offset = 1.0 / textureSize(texture1, 0);
    vec4 result = vec4(0.0);
    float k = 0;
    int points = 30;
    float inc = 2.f * uSize / float(points);
    for (int x = 0; x < points; x++)
    for (int y = 0; y < points; y++)
    {
        float weight = 1.0;
        vec2 tc = vec2(
            float(x) * inc - uSize,
            float(y) * inc - uSize
        ) * tex_offset;
        weight = 1.0 - abs(tc.x) * abs(tc.y);
        tc += oTexCoord;
        if (tc.x < 0.0 || tc.x > 1.0 || tc.y < 0.0 || tc.y > 1.0)
            continue;
        result += texture(texture1, tc) * weight;
        k += weight;
    }

    result /= float(k);
    FragColor = result;
}
