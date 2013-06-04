#version 330 core

uniform mat4 proj;

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
  gl_Position = proj * vec4(v[gl_VertexID], 1.);
  vno = floor(v[gl_VertexiID]);
}
