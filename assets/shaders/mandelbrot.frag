#version 330 core

in vec2 uv;

layout (location = 0) out uint r;

uniform uint iterations;

void main()
{
    vec2 x = vec2(0);
    uint i;
    bool in_set = true;
    for (i = 0u; i < iterations; )
    {
        x = vec2(x.x * x.x - x.y * x.y + uv.x, 2.0 * x.x * x.y + uv.y);
        float d = x.x * x.x + x.y * x.y;
        i++;
        if (d > 4.0)
        {
            in_set = false;
            break;
        }
    }
    r = in_set ? 0u : i;
}