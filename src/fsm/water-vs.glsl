#version 330 core

layout (location = 0) in vec3 co;

out vec3 vco;
out vec3 vno;

uniform mat4 proj;
uniform mat4 view;
uniform float time;

const float h = 0.0001;

float water(vec2 xy) {
  float w =
      sin(xy.x*8.)
    + sin(xy.y*8.)
    + sin(length(xy)*10.)
    ;
  return w / 3.;
}

vec3 deriv_no(vec2 xy, float h) {
  float wxy = water(xy);
  float dx = water(vec2(xy.x+h, xy.y)) - wxy;
  float dy = water(vec2(xy.x, xy.y+h)) - wxy;

  return normalize(vec3(-dx, -dy, h));
}

void main() {
  vec2 lookup = co.xy*0.2+time*0.5;
  vco = vec3(co.x, water(co.xy*0.2+time*0.5)*0.5, co.y);
  vco.y -= 2.; /* height */
  vno = deriv_no(lookup, h); 

  gl_Position = proj * view * vec4(vco, 1.);
}

