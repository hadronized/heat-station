#version 330 core

out vec3 vCo;

uniform vec2 vnb; /* vertices nb; 1 / vertices nb */
uniform float t;

const float PI = 3.141592;

void main() {
  /* laser */
  float d = 10.;
  float d2 = d * 0.5;
  vCo = vec3(0., 0., gl_VertexID*vnb.y*d - d2);

  /* displacement */
  vCo.y += sin(vCo.z*2.*PI+t*20.)/30.;
  vCo.y += clamp(tan(vCo.z+t)/300., -2., 2.);
}
