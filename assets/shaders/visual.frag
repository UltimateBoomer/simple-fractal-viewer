#version 330 core

in vec2 uv;

out vec3 color;

uniform usampler2D tex;
uniform float hue;

vec3 hsv_to_rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
  uint d = texture(tex, uv).r;
  color = 0.75 * hsv_to_rgb(vec3(hue + log(d), 1.0f, 1.0f));
}