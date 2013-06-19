#version 330 core

in vec3 vco;
in vec3 vno;

out vec4 frag;

void main() {
  float faceid = float(gl_PrimitiveID / 2);
  frag = vec4(1. - sin(faceid), cos(faceid), 0., 1.);

  /* laser phong */
  float laserDist    = length(vco.xy);
  vec4 laserDiffuse  = vec4(0.75, 0., 0., 1.);
  vec4 liquidDiffuse = vec4(0.035, 0.412, 0.635, 1.);

  frag = liquidDiffuse / log(pow(laserDist, 2.)) * 0.5;
  frag.w = 0.4; /* water opacity */
}
