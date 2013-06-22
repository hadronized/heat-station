#version 330 core

layout (location = 0) in vec3 co;

out vec3 vco;
out vec3 vno;

uniform mat4 proj;
uniform mat4 view;
uniform float time;

const float h = 0.0001;
const float a = 0.5;

float water(vec2 xy) {
  float w =
      sin(xy.x*8.)
    + sin(xy.y*8.)
    + sin(length(xy)*10.+time*6.)
    ;
  return w / 3. * a;
}

vec3 deriv_no(vec2 xz, float h) {
  float wxz = water(xz);
  float dx = water(vec2(xz.x+h, xz.y)) - wxz;
  float dz = water(vec2(xz.x, xz.y+h)) - wxz;

  return normalize(vec3(-dx, h, -dz));
}

void main() {

  vec2 lookup = co.xy*0.2;
  vco = vec3(co.x, water(lookup) - 2., co.y);
  vno = deriv_no(lookup, h); 

  gl_Position = proj * view * vec4(vco, 1.);
}


/*

1    0    dx
dx × dz = -1
0    1    dz

*/
