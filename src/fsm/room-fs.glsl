#version 330 core

uniform mat4 proj; /* used to retrieve camera position */
uniform float t;   /* time */

in vec3 vco;       /* vertex shader space coordinates */
in vec3 vno;  /* vertex shader normal */

out vec4 frag;

void main() {
  vec3 eye  = vec3(0., 0., 0.); /* camera position */
  vec3 lpos = vec3(cos(t)*2., sin(t)*2., 0.); /* light pos */
  vec3 lvco = lpos - vco;
  vec3 ldir = normalize(lvco); /* light ray */

  frag = vec4(1.) * max(0., dot(vno, ldir)) / sqrt(length(lvco)) * 0.5;
}
