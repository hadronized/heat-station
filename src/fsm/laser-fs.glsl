#version 330 core

in vec2 gUV;
in float gHeight;

out vec4 frag;

uniform sampler2D lasertex;

void main() {
  frag = texture(lasertex, gUV) + vec4(0., 0., 1., 1.) * gHeight/2.;
}
