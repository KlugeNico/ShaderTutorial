#version 330 core

uniform vec2 resolution;    // @FLOAT
uniform sampler2D tex;

in vec2 UV;

// Ouput data
out vec3 color;

vec3 getColor();

void main()
{
    // Ameisen sind rot.
    // Sehr kompliziert rumgerechnet. Letzendlich werden an Ballungszentren Ameisen dadurch eher weiß statt Rot.
    vec2 uv = vec2( UV.x - 0.5f, UV.y - 0.5f );
    float r = (1.f - length(uv) * 2.f) * 0.5f;
    if (r < 0.0f) r = 0.0f;
    vec3 col = getColor();
    float g = 0.3f * r;
    float b = 0f;
    if ( (col.r > 0.9f && col.g > 0.9f) ){
        b = 0.1f;
        g = 0.0f;
        r = 0.0f;
    }
    color = vec3(r, g, b);
}

vec3 getColor()
{
    vec2 position = gl_FragCoord.xy;
    if (position.x < 0) position.x = resolution.x - 1;
    if (position.y < 0) position.y = resolution.y - 1;
    if (position.x >= resolution.x) position.x = 0;
    if (position.y >= resolution.y) position.y = 0;
    return texture2D(tex, position / resolution.xy).xyz;
}
