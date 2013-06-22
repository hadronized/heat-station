#version 330 core

in vec3 vco;
in vec3 vno;

out vec4 frag;

uniform mat4 view;

void main() {
  vec3 eye = inverse(view)[3].xyz;
  float faceid = float(gl_PrimitiveID / 2);
  frag = vec4(1. - sin(faceid), cos(faceid), 0., 1.);

  /* laser phong */
  float laserDist    = length(vco.xy);
  vec4 laserDiffuse  = vec4(0.75, 0., 0., 1.);
  vec4 liquidDiffuse = vec4(0.035, 0.412, 0.635, 1.);
  frag = liquidDiffuse / log(pow(laserDist, 2.)) * 0.5;

  /* water phong */

  frag.w = 0.4; /* water opacity */
  
  frag = vec4(vno, 1.);
}
