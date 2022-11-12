#version 330 core

in vec2 UV;

// Ouput data
out vec3 color;

void main()
{
    vec2 uv = vec2( UV.x - 0.5f, UV.y - 0.5f );
    float c = 1.0f - length(uv) * 2.0f;
    if (c < 0.0f) c = 0.0f;
    color = vec3(
        c,
        c,
        c
    );
}