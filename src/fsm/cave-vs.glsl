#version 330 core

in vec3 co;
out vec3 vno;

uniform sampler2D heightmap;
uniform mat4 proj;
uniform mat4 view;
uniform float t;

const float h = 1./6.;

vec3 deriv_no(vec2 xz, float first) {
  float wxz = first;
  float dx = texture(heightmap, vec2(xz.x+h, xz.y)).r - wxz;
  float dz = texture(heightmap, vec2(xz.x, xz.y+h)).r - wxz;

  return normalize(vec3(-dx, h, -dz));
}

void main() {
  /* compute space coordinates */
  vec3 pos = co.xxy;
  vec2 lookup = (co.xy/6.+1.)*0.5;
  pos.y = texture(heightmap, lookup).r;
  /* compute normal */
  vno = deriv_no(lookup, pos.y);

  gl_Position = proj * view * vec4(pos, 1.);
}

