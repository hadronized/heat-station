#version 330 core

out vec3 vCo;
uniform vec2 vnb; /* vertices nb; 1 / vertices nb */
uniform float t;

const float PI = 3.141592;

void main() {
  /* laser */
  vCo = vec3(0., 0., -(vnb.x*.5 - gl_VertexID*vnb.y*10.));

  /* displacement */
  vCo.xy += vec2(cos(t)*1., sin(t)*1.);
  vCo.y += sin(vCo.z*2.*PI+t*20.)/30.;
  vCo.z -= 3.;
}
