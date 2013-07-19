#version 330 core

in vec3 co;
out vec3 vno;

uniform sampler2D heightmap;
uniform sampler2D heightmap2;
uniform mat4 proj;
uniform mat4 view;
uniform vec4 pres;
uniform float t;

vec2 h = vec2(1. / 256.);
float maxAmp = sin(t*0.1); /* max amplitude */

float height(vec2 uv) {
  return texture(heightmap, uv).r * maxAmp;
}

vec3 deriv_no(vec2 xz, float first) {
  float wxz = first;
  float dx = height(vec2(xz.x+h.x, xz.y)) - wxz;
  float dz = height(vec2(xz.x, xz.y+h.y)) - wxz;

  return normalize(vec3(-dx, h.y, -dz));
}

void main() {
  /* compute space coordinates */
  vec3 pos = co.xxy;
  vec2 lookup = (co.xy+pres.xy*0.5)*pres.zw;

  pos.y = height(lookup);
  vno = deriv_no(lookup, pos.y);
  //vno = vec3(lookup, 0.);

  gl_Position = proj * view * vec4(pos, 1.);
}

