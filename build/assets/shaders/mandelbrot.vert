#version 330 core

layout (location = 0) in vec2 pos_in;

out vec2 uv;

uniform mat3 draw_tf;

void main()
{
    gl_Position = vec4(pos_in, 0.0, 1.0);
    uv = (draw_tf * vec3(pos_in, 1.0)).xy;
}   
