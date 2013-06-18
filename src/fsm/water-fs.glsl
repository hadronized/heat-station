#version 330 core

in vec3 vco;
in vec3 vno;

out vec4 frag;

void main() {
  float faceid = float(gl_PrimitiveID / 2);
  frag = vec4(1. - sin(faceid), cos(faceid), 0., 1.);

  /* laser phong */
  float laserDist = length(vco.xy);

  frag /= laserDist * 0.1;
}
