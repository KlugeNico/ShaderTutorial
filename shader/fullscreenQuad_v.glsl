#version 330 core

out vec2 UV;

vec4 normalQuad(int id);
vec2 uvQuad(int id);

void main()
{
    UV = uvQuad(gl_VertexID % 4);
    gl_Position = normalQuad(gl_VertexID);
}

vec4 normalQuad(int id)
{
    switch(id) {
        case 0: return vec4(-1.0f, -1.0f, 0.0f, 1.0f);
        case 1: return vec4(-1.0f, 1.0f, 0.0f, 1.0f);
        case 2: return vec4(1.0f, 1.0f, 0.0f, 1.0f);
        case 3: return vec4(1.0f, -1.0f, 0.0f, 1.0f);
    }
    return vec4(-1.0f, -1.0f, 0.0f, 1.0f);
}

vec2 uvQuad(int id)
{
    switch(id) {
        case 0: return vec2(0f, 0f);
        case 1: return vec2(0.0f, 1.0f);
        case 2: return vec2(1.0f, 1.0f);
        case 3: return vec2(1.0f, 0.0f);
    }
    return vec2(0.0f, 0.0f);
}