#version 330 core

uniform mat4 proj;

out vec3 vco;      /* space coordinates as vertex shader output */
flat out vec3 vno; /* normal as vertex shader output */

/* cube vertices */
const vec3[8] v = vec3[] (
    vec3(-1.,  1.,  1.)
  , vec3( 1.,  1.,  1.)
  , vec3( 1., -1.,  1.)
  , vec3(-1., -1.,  1.)
  , vec3(-1.,  1., -1.)
  , vec3( 1.,  1., -1.)
  , vec3( 1., -1., -1.)
  , vec3(-1., -1., -1.)
);

void main() {
  vco = v[gl_VertexID];
  vno = floor(vco);

  gl_Position = proj * vec4(vco, 1.);
}
