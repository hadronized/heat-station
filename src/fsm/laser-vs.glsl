#version 330 core

uniform vec2 vnb; /* vertices nb; 1 / vertices nb */
uniform mat4 proj;
uniform mat4 view;
uniform float t;

const float PI = 3.141592;

void main() {
  /* laser */
  vec3 p = vec3(0., 0., -gl_VertexID*vnb.y*1.);
  p.z += vnb.x * .5;

  /* displacement */
  p.y += sin(p.z*2.*PI+t*20.)/30.;

  gl_Position = proj * view * vec4(p, 1.);
}
