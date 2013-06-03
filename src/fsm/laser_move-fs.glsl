#version 330 core

uniform vec4 res;
uniform sampler2D srctex;
uniform float t;

out vec4 frag;

void main() {
  vec4 src = texture2D(srctex, gl_FragCoord.xy * res.zw);

  frag = src;
}
