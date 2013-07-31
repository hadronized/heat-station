#version 330 core

out vec3 vCo;

uniform vec2 vnb; /* vertices nb; 1 / vertices nb */
uniform float t;

const float PI = 3.141592;

void main() {
  /* laser */
  vCo = vec3(0., 0., gl_VertexID*vnb.y*8.5 - 4.25);

  /* displacement */
  vCo.y += sin(vCo.z*2.*PI+t*20.)/30.;
  vCo.y += clamp(tan(vCo.z+t)/300., -2., 2.);
}
