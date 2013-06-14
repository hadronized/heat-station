#version 330 core

out vec3 vco; /* space coordinates as vertex shader output */
out vec3 vno; /* normal as vertex shader output */

uniform float size;      /* size of the slab */
uniform float thickness; /* thickness of the slab: 0. = 0., 1. = size */

/* slab vertices */
const float margin = 0.05;
float offset = size + margin;
float size_2  = size / 2.;
float depth_2 = size_2 * thickness;
vec3[8] v = vec3[] (
    vec3(-size_2,  size_2,  depth_2)
  , vec3( size_2,  size_2,  depth_2)
  , vec3( size_2, -size_2,  depth_2)
  , vec3(-size_2, -size_2,  depth_2)
  , vec3(-size_2,  size_2, -depth_2)
  , vec3( size_2,  size_2, -depth_2)
  , vec3( size_2, -size_2, -depth_2)
  , vec3(-size_2, -size_2, -depth_2)
);

void main() {
  float foo = offset * 10.;
  vec2 c = vec2(10*size+9*margin,5*size+4*margin) * 0.5 + size;
  vco = v[gl_VertexID];

  if (gl_InstanceID < 50) {
    vco += vec3(mod(gl_InstanceID, 10)*offset, floor(gl_InstanceID/10)*offset, 0) - vec3(c, c.x);
  } else if (gl_InstanceID < 100) {
    vco += vec3(mod(gl_InstanceID-50, 10)*offset, floor((gl_InstanceID-50)/10)*offset, foo) - vec3(c, c.x);
  } else if (gl_InstanceID < 150) {
    vco += vec3(0., floor((gl_InstanceID-100)/10)*offset, mod(gl_InstanceID-100, 10)*offset) - vec3(c, c.x);
  } else if (gl_InstanceID < 200) {
    vco += vec3(foo, floor((gl_InstanceID-150)/10)*offset, mod(gl_InstanceID-150, 10)*offset) - vec3(offset*10, offset*5, offset*10)*0.5 + size;

  } else if (gl_InstanceID < 250) {
    vco += vec3(mod(gl_InstanceID-200, 10)*offset, 0., floor((gl_InstanceID-200)/10)*offset);
  } else {
    vco += vec3(mod(gl_InstanceID-250, 10)*offset, foo, floor((gl_InstanceID-250)/10)*offset);
  }
}
