#version 330 core

uniform mat4 proj;
uniform float size;      /* size of the slab */
uniform float thickness; /* thickness of the slab: 0. = 0., 1. = size */
uniform float t;

out vec3 vco;      /* space coordinates as vertex shader output */
out vec3 vno; /* normal as vertex shader output */

/* slab vertices */
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
  vco = v[gl_VertexID];
  vno = floor(normalize(vco) + 1.);
  
  if (gl_InstanceID < 25) {
    vco += vec3(-mod(gl_InstanceID, 5)*(size+0.1), -floor(gl_InstanceID/5)*(size+0.1), -5.);
  } else if (gl_InstanceID < 50) {
    vco += vec3(-mod(gl_InstanceID-25, 5)*(size+0.1), -floor((gl_InstanceID-25)/5)*(size+0.1), 0.);
  } else if (gl_InstanceID < 75) {
    vco += vec3(-5., -floor((gl_InstanceID-50)/5)*(size+0.1), -mod(gl_InstanceID-50, 5)*(size+0.1));
  } else if (gl_InstanceID < 100) {
    vco += vec3(0., -floor((gl_InstanceID-75)/5)*(size+0.1), -mod(gl_InstanceID-75, 5)*(size+0.1));
  } else if (gl_InstanceID < 125) {
    vco += vec3(-mod(gl_InstanceID-100, 5)*(size+0.1), -5., -floor((gl_InstanceID-100)/5)*(size+0.1));
  } else {
    vco += vec3(-mod(gl_InstanceID-125, 5)*(size+0.1), 0., -floor((gl_InstanceID-125)/5)*(size+0.1));
  }

  vco.x += cos(t)*10.;
  vco.y += sin(t)*10.;
  /* TODO: move vco according to gl_InstanceID */
  gl_Position = proj * vec4(vco, 1.);
}
