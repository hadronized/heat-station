#version 330 core

in vec3 vno;

layout (location = 0) out vec3 nofrag;
layout (location = 1) out ivec2 matfrag;

uniform sampler2D heightmap;
uniform vec4 pres;

void main() {
  nofrag = vno;
  matfrag = ivec2(2u, 2u);
}
