#version 330 core

out vec4 frag;

uniform vec4 res;
uniform float t;

const float PI = 3.14159265359;

float laser_tex(vec2 uv, float i) {
  return pow(sin(uv.x*PI), i);
}

float rand(vec2 a){
  return sin(dot(a.xy,vec2(12.9898,78.233)))*43758.5453;
}

void main() {
  vec2 uv = gl_FragCoord.xy * res.zw;
  vec4 laserColor = vec4(0.75, 0., 0., 1.);
  vec4 laserCoreColor = vec4(1.);

  frag = laserColor * laser_tex(uv, 4.);
  frag += laserCoreColor * laser_tex(uv, 24.) * 0.42;
}
