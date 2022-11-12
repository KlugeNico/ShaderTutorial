#version 330 core

// @IN_BUFFER ANTS
// @OUT_BUFFER ANTS

layout(location = 0) in vec2 positionIn;    // @FLOAT
layout(location = 1) in float angleIn;    // @FLOAT

out vec2 positionOut;   // @GRAP
out float angleOut;   // @GRAP

uniform uint randInValue;    // @UINT
uniform vec2 resolution;    // @FLOAT
uniform sampler2D tex;    // @TEXTURE

vec3 sense(float angle, float distance);
vec2 bound(vec2 position);
vec3 getColor(vec2 offset);
float randF();

const float senseAngle = 0.3f;
const float senseDistance = 8.0f;
const float offroadMul = 0.1f;
const float senseMul = 0.1f;

void main()
{
    vec2 direction = vec2(cos(angleIn), sin(angleIn));
    positionOut = bound(positionIn + direction);
    vec3 leftSense = sense(angleIn - senseAngle, senseDistance);
    vec3 rightSense = sense(angleIn + senseAngle, senseDistance);
    float randOffRoad = (randF() - 0.5f) * offroadMul;
    // Ballungszentren meiden:
    float senseAngle = (rightSense.r - rightSense.b) - (leftSense.r - leftSense.b);
    // Ballungszentren nicht meiden:
    //float senseAngle = rightSense.r - leftSense.r;
    angleOut = angleIn + senseAngle * senseMul;
}

vec3 sense(float angle, float distance)
{
    vec2 offset = vec2(cos(angle), sin(angle)) * distance;
    vec3 colorSum = vec3(0,0,0);
    for (int ix = -1; ix < 2; ix++) {
        for (int iy = -1; iy < 2; iy++) {
            colorSum += getColor(offset + vec2(ix, iy));
        }
    }
    colorSum /= 9;
    return colorSum;
}

vec3 getColor(vec2 offset)
{
    vec2 position = bound(positionIn + offset);
    return texture2D(tex, position / resolution.xy).xyz;
}

vec2 bound(vec2 position)
{
    vec2 pos = position;
    if (pos.x < 0) pos.x = resolution.x - 1;
    if (pos.y < 0) pos.y = resolution.y - 1;
    if (pos.x >= resolution.x) pos.x = 0;
    if (pos.y >= resolution.y) pos.y = 0;
    return pos;
}

float randF()
{
    uint inValue = uint(gl_VertexID) + randInValue;
    uint state = inValue;
    state ^= 2747636419u;
    state *= 2654435769u;
    state ^= state >> 16;
    state *= 2654435769u;
    state ^= state >> 16;
    state *= 2654435769u;
    return float(state % 1000000u) / 1000000.0f;
}