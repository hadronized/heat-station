#version 330 core

in vec3 gco; /* vertex shader space coordinates */
in vec3 gno; /* vertex shader normal */

layout (location = 0) out vec3 nofrag;
layout (location = 1) out uvec2 matfrag;

void main() {

  nofrag = gno;
  matfrag = uvec2(1, 0);
}
