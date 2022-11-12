#version 330 core

uniform vec2 resolution;
uniform sampler2D tex;

// Ouput data
out vec3 color;

// functions
vec3 getColor(vec2 offset);
vec3 pixelInfluence(vec2 offset);
uint rand(uint seed);

void main()
{
    vec2 position = gl_FragCoord.xy / resolution;
    uint posX = uint(gl_FragCoord.x);
    uint posY = uint(gl_FragCoord.y);
    uint screenWidth = uint(resolution.x);
    uint seed = (posY * screenWidth + posX) * 3u;

    color = vec3(
        (rand(seed) % 255u) / 255.0f,
        (rand(seed + 1u) % 255u) / 255.0f,
        (rand(seed + 2u) % 255u) / 255.0f
    );

}

uint rand(uint seed)
{
    uint state = seed;
    state ^= 2747636419u;
    state *= 2654435769u;
    state ^= state >> 16;
    state *= 2654435769u;
    state ^= state >> 16;
    state *= 2654435769u;
    return state;
}

vec3 getColor(vec2 offset)
{
    vec2 position = gl_FragCoord.xy + offset;
    if (position.x < 0) position.x = resolution.x - 1;
    if (position.y < 0) position.y = resolution.y - 1;
    if (position.x >= resolution.x) position.x = 0;
    if (position.y >= resolution.y) position.y = 0;
    return texture2D(tex, position / resolution.xy).xyz;
}

vec3 pixelInfluence(vec2 offset)
{
    vec3 pixColor = getColor(offset);
    vec3 xColor = pixColor.x * offset.x * pixColor;
    vec3 yColor = pixColor.y * offset.y * pixColor;
    return xColor + yColor;
}