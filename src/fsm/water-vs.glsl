#version 330 core

layout (location = 0) in vec3 co;

out vec3 vco;
out vec3 vno;

uniform mat4 proj;
uniform mat4 view;
uniform float time;

float water(vec2 xy) {
  float w =
      sin(xy.x*8.)
    + sin(xy.y*8.)
    + sin(length(xy)*10.)
    ;
  return w / 3.;
}

void main() {
  vco = vec3(co.x, water(co.xy+time*0.25)*0.5, co.y);
  vco.y -= 2.; /* height */
  vno = vec3(0., 1., 0.);

  gl_Position = proj * view * vec4(vco, 1.);
}

