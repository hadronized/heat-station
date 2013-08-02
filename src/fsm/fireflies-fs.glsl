#version 330 core

flat in vec3 gcolor;
in vec2 gco;
out vec4 frag;

void main() {
  float d = (1. - sqrt(length(gco)));
  frag = vec4(gcolor, 1.) * d * 2.5;//vec4(2.4, 2., 3.6, 4.);
}

