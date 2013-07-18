#version 330 core

in vec3 vno;

layout (location = 0) out vec3 nofrag;
layout (location = 1) out ivec2 matfrag;

void main() {
  nofrag = vno;
  matfrag = ivec2(1u, 2u);
}
