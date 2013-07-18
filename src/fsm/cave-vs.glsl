#version 330 core

in vec3 co;
out vec3 vno;

uniform sampler2D heightmap;
uniform mat4 proj;
uniform mat4 view;
uniform float t;

const float h = 0.0001;
const float c = 0.001;

vec3 deriv_no(vec2 xz) {
  float wxz = texture(heightmap, xz);
  float dx = texture(heightmap, vec2(xz.x+h, xz.y)) - wxz;
  float dz = texture(heightmap, vec2(xz.x, xz.y+h)) - wxz;

  return normalize(vec3(-dx, h, -dz));
}

void main() {
  /* compute space coordinates */
  vec3 pos = co;
  co.y = texture(heightmap, co.xz*c).r;
  /* compute normal */
  vno = deriv_no(co.xz*c);

  gl_Position = proj * view * vec4(co, 1.);
}

