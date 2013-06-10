#version 330 core

in vec2 gUV;
out vec4 frag;

uniform sampler2D lasertex;

void main() {
  frag = texture2D(lasertex, gUV);
}
