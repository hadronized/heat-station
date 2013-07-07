#version 330 core

in vec3 vco;
in vec3 vno;

uniform mat4 view;

layout (location = 0) out vec3 nofrag;
layout (location = 1) out uvec2 matfrag;

void main() {
  vec3 eye = inverse(view)[3].xyz;
  float faceid = float(gl_PrimitiveID / 2);
  /*
  frag = vec4(1. - sin(faceid), cos(faceid), 0., 1.);
  */

  /* laser phong */
  vec4 laserDiffuse  = vec4(0.75, 0., 0., 1.);

  /* water phong */
  //vec4 liquidDiffuse = vec4(0.035, 0.412, 0.635, 1.);
  vec4 liquidDiffuse = vec4(0.4, 0.4, 0.4, 1.);
  float laserHeight = 0.;
  vec3 ldir = vec3(0., laserHeight, 0.) - vec3(vco.xy, 0.);
  float laserDist = length(vco.xy);
  vec3 nldir = normalize(ldir);
  float waterDiffuse = max(0., dot(nldir, vno));
  float waterSpecular = pow(max(0., dot(reflect(normalize(vco-eye), vno), nldir)), 10.);

#if 0
  frag =  liquidDiffuse * waterDiffuse / (log(laserDist)*2.5);
  frag += laserDiffuse  * waterDiffuse / (pow(laserDist, 4.)*0.1);
  frag += laserDiffuse  * waterSpecular;
#endif
  
  nofrag    = vno;
  matfrag   = uvec2(1, 1);
}
