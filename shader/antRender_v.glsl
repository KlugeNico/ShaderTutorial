#version 330 core

layout(location = 0) in vec2 position;  // @FLOAT
layout(location = 1) in float angle;  // @FLOAT

uniform vec2 resolution;    // @FLOAT
uniform sampler2D tex;

out vec2 UV;

vec2 normalQuad(int id);
vec2 uvQuad(int id);


void main()
{
    UV = uvQuad(gl_VertexID % 4);
    gl_Position = vec4( ( ( position / resolution ) * 2.0f - 1.0f ) + normalQuad( gl_VertexID % 4 ), 0.0f, 1.0f );
}


vec2 normalQuad(int id)
{
    switch(id) {
        case 0: return 4.0f * vec2(-1.0f, -1.0f) / resolution;
        case 1: return 4.0f * vec2(-1.0f, 1.0f) / resolution;
        case 2: return 4.0f * vec2(1.0f, 1.0f) / resolution;
        case 3: return 4.0f * vec2(1.0f, -1.0f) / resolution;
    }
    return vec2(-1.0f, -1.0f);  // should never happen
}

vec2 uvQuad(int id)
{
    switch(id) {
        case 0: return vec2(0.0f, 0.0f);
        case 1: return vec2(1.0f, 0.0f);
        case 2: return vec2(1.0f, 1.0f);
        case 3: return vec2(0.0f, 1.0f);
    }
    return vec2(0.0f, 0.0f);  // should never happen
}