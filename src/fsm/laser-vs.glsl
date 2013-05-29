#version 330 core

uniform float rvnb; /* 1 / vertices nb */
uniform mat4 proj;

void main() {
  vec3 p =vec3(0., 0., gl_VertexID*rvnb);
  p -= vec3(.2); /* cam */
  gl_Position = proj * vec4(p, 1.);
}
