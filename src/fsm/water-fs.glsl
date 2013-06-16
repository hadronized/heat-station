#version 330 core

out vec4 frag;

uniform vec4 res;
uniform float t;
uniform float fovy;

vec2 rand2(vec2 co, float seed) {
  return (vec2(fract(sin(dot(co.xy, vec2(12.9898, 78.233)*(1.+seed))) * 43858.5453),
               fract(sin(dot(-co.xy, vec2(-78.8765, 0.764)*(2.+seed))) * 34890.8524)) - .5) * 2.;
}

vec2 get_uv() {
  vec2 uv = 2. * gl_FragCoord.xy * res.zw - 1.;
  uv.y *= res.y * res.z;

  return uv;
}

vec3 get_ray(vec2 uv) {
  return normalize(vec3(uv, 1. / tan(fovy)));
}

float water(vec2 co, float seed) {
  return
      sin(co.x*8.) + sin(co.y*8.)
    + sin(length(co)*12.)
    ;
}

void main() {
  vec2 uv = get_uv();
  vec3 ray = get_ray(uv);

  float w0 = water(uv+sin(t), 0.);
  frag = vec4(w0, w0/2., w0*sin(t), 1.);
}
