#version 330 core

uniform mat4 view;

in vec3 gco; /* vertex shader space coordinates */
in vec3 gno; /* vertex shader normal */

out vec4 frag;

void main() {
  vec3 eye  = inverse(view)[3].xyz; /* camera position */
  vec3 lpos = vec3(0., 0., 0.); /* light pos */
  vec3 lvco = lpos - gco;
  vec3 ldir = normalize(lvco); /* light ray */

  frag = vec4(1.) * max(0., dot(gno, ldir)) / sqrt(length(lvco)) * 0.5;
  //frag = vec4(gno, 1.);
}
