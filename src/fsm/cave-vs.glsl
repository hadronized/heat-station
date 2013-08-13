#version 330 core

in vec3 co;
out vec3 vno;

uniform sampler2D heightmap;
uniform sampler2D heightmap2;
uniform mat4 proj;
uniform mat4 view;
uniform vec4 pres;
uniform float t;

vec2 h = vec2(1. / 512.);
float maxAmp = 4.;//4.*sin(t*0.1); /* max amplitude */

float height(vec2 uv) {
  return texture(heightmap, uv).r * maxAmp;
}

float height2(vec2 uv) {
  return texture(heightmap2, uv).r * maxAmp;
}

vec3 deriv_no(vec2 xz, float first) {
  float wxz = first;
  float dx = height(vec2(xz.x+h.x, xz.y)) - wxz;
  float dz = height(vec2(xz.x, xz.y+h.y)) - wxz;

  return normalize(vec3(-dx, h.y, -dz));
}

vec3 deriv_no2(vec2 xz, float first) {
  float wxz = first;
  float dx = height2(vec2(xz.x+h.x, xz.y)) - wxz;
  float dz = height2(vec2(xz.x, xz.y+h.y)) - wxz;

  return normalize(vec3(-dx, h.y, -dz));
}

float mixer(float a) {
  return a;
  return clamp(pow(a, 2.)*pow(-a+0.5, 2.), 0., 1.);
}

void main() {
  /* compute space coordinates */
  vec3 pos = co.xxy;
  vec2 lookup = (co.xy+pres.xy*0.5)*pres.zw;
  lookup = co.xy*0.1;

  float mixed = mixer(clamp(t-112., 0., 1.));
  pos.y = mix(height(lookup), height2(lookup), mixed);
  vno = mix(deriv_no(lookup, pos.y), deriv_no2(lookup, pos.y), mixed);
  vno *= (gl_InstanceID == 0 ? 1 : -1);
  pos.y += 4. * (gl_InstanceID == 0 ? 1 : -1);

  gl_Position = proj * view * vec4(pos, 1.);
}

