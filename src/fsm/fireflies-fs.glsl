#version 330 core

flat in vec3 gcolor;
in vec2 gco;
out vec4 frag;

void main() {
  frag = vec4(gcolor, 1.) * (1. - length(gco));
}

