#version 330 core

flat in vec3 gcolor;
out vec4 frag;

void main() {
  frag = vec4(gcolor, 1.);
}

