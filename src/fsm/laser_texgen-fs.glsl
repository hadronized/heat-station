#version 330 core

out vec4 frag;

uniform vec4 res;
uniform float t;

const float PI = 3.14159265359;

float laser_tex(vec2 uv, float i) {
  return pow(sin(uv.x*PI), i);
}

void main() {
  vec2 uv = gl_FragCoord.xy * res.zw;
  vec4 laserColor = vec4(0.75, 0., 0., 1.);
  vec4 laserCoreColor = vec4(1.);

  frag = laserColor * laser_tex(uv, 4.) * 1.5;
  frag += laserCoreColor * laser_tex(uv, 14.) * 1.02;
}
