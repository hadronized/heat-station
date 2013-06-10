#version 330 core

uniform sampler2D srctex;
uniform vec4 res;

out vec4 frag;

void main() {
  vec2 uv = gl_FragCoord.xy * res.zw;
  vec2 st = vec2(0., res.w*1.5);

  frag = texture2D(srctex, uv - 6.*st) * 0.05
       + texture2D(srctex, uv - 2.*st)  * 0.25
       + texture2D(srctex, uv)          * 0.45
       + texture2D(srctex, uv + 2.*st)  * 0.25
       + texture2D(srctex, uv + 6.*st) * 0.05
       ;
}
