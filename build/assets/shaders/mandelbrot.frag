#version 330 core

in vec2 uv;

layout (location = 0) out float r;

uniform int iterations;

void main()
{
    vec2 x = vec2(0);
    int i;
    bool in_set = true;
    for (i = 0; i < iterations; )
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
    r = in_set ? 0.0 : log2(i) / 8;
}