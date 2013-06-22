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
  vec3 lightPos = vec3(0., 50., 0.);
  vec3 ldir = lightPos - vco;
  vec3 nldir = normalize(ldir);
  float waterDiffuse = max(0., dot(nldir, vno));
  float waterSpecular = pow(max(0., dot(reflect(normalize(vco-eye), vno), nldir)), 10.);
  frag = vec4(1.) * waterDiffuse + vec4(0.75, 0., 0., 1.) * waterSpecular;
  frag /= laserDist * 1.;
  
  //frag = vec4(vno*0.5 + 0.5, 1.);

  //frag.w = 0.4; /* water opacity */
}
