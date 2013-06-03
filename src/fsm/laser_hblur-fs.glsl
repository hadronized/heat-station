#version 330 core

uniform sampler2D srctex;
uniform vec4 res;

out vec4 frag;

void main() {
  vec2 uv = gl_FragCoord.xy * res.zw;
  vec2 st = vec2(res.z+0.5, 0.);

  frag = texture2D(srctex, uv - 3.*st) * 0.15
       + texture2D(srctex, uv - 2.*st) * 0.25
       + texture2D(srctex, uv - st)    * 0.4
       + texture2D(srctex, uv)         * 0.5
       + texture2D(srctex, uv + st)    * 0.4
       + texture2D(srctex, uv + 2.*st) * 0.25
       + texture2D(srctex, uv + 3.*st) * 0.15
       ;
}
