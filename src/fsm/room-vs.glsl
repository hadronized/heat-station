#version 330 core

out vec3 vco; /* space coordinates as vertex shader output */

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
  float foo = offset * 5.;
  vco = v[gl_VertexID];

  if (gl_InstanceID < 25) {
    vco += vec3(mod(gl_InstanceID, 5)*offset, floor(gl_InstanceID/5)*offset, 0.);
  } else if (gl_InstanceID < 50) {
    vco += vec3(mod(gl_InstanceID-25, 5)*offset, floor((gl_InstanceID-25)/5)*offset, foo);
  } else if (gl_InstanceID < 75) {
    vco += vec3(0., floor((gl_InstanceID-50)/5)*offset, mod(gl_InstanceID-50, 5)*offset);
  } else if (gl_InstanceID < 100) {
    vco += vec3(foo, floor((gl_InstanceID-75)/5)*offset, mod(gl_InstanceID-75, 5)*offset);
  } else if (gl_InstanceID < 125) {
    vco += vec3(mod(gl_InstanceID-100, 5)*offset, 0., floor((gl_InstanceID-100)/5)*offset);
  } else {
    vco += vec3(mod(gl_InstanceID-125, 5)*offset, foo, floor((gl_InstanceID-125)/5)*offset);
  }
  vco -= foo * 0.5;
}
