#version 330 core

in vec2 UV;

out vec3 color;

uniform vec2 resolution;
uniform sampler2D tex;

vec3 getColor(vec2 offset);

void main(){
    vec3 colorSum = vec3(0,0,0);
    for (int ix = -1; ix < 2; ix++) {
        for (int iy = -1; iy < 2; iy++) {
            colorSum += getColor(vec2(ix, iy));
        }
    }
    colorSum /= 9;
    colorSum -= 0.04f;
    color = colorSum;
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
