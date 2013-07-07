#version 330 core

uniform mat4 view;

in vec3 gco; /* vertex shader space coordinates */
in vec3 gno; /* vertex shader normal */

layout (location = 0) out vec3 nofrag;
layout (location = 1) out uvec2 matfrag;

void main() {
  vec3 eye  = inverse(view)[3].xyz; /* camera position */
  vec3 lvco = -vec3(gco.xy, 0.);
  vec3 ldir = normalize(lvco); /* light ray */
  float laserDist = length(lvco);
  float diffuse = max(0., dot(gno, ldir));

  /*
  frag = vec4(0.6) * diffuse / (log(laserDist)*2.5);
  frag += vec4(0.75, 0., 0., 1.) * diffuse / (pow(laserDist, 4.) * 0.1);
  */

  nofrag = gno;
  matfrag = uvec2(1, 0);
}
