#version 330 core

uniform mat4 proj; /* used to retrieve camera position */

in vec3 vco;      /* vertex shader space coordinates */
flat in vec3 vno; /* vertex shader normal */

out vec4 frag;

void main() {
  /* first, simple diffuse phong with origined light */
  frag = 

