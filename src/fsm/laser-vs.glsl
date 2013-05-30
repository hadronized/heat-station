#version 330 core

uniform float rvnb; /* 1 / vertices nb */
uniform mat4 proj;
uniform float t;

const float PI = 3.141592;

void main() {
  vec3 p =vec3(0., 0., -gl_VertexID*rvnb*1.);
  p -= vec3(0.3+sin(t), 0., 0.1); /* cam */
  p.y += sin(p.z*2.*PI+t*20.)/30.; /* displacement */
  gl_Position = proj * vec4(p, 1.);
}
