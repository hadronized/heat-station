#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vco[];
in vec3 vno[];

out vec3 gco;
out vec3 gno;
out vec2 guv;

uniform mat4 proj;
uniform mat4 view;

void emit(int i) {
  gco = vco[i];
  gl_Position = proj * view * vec4(gco, 1.);
  EmitVertex();
}

void main() {
  /* cube face ID */
  int faceID = gl_PrimitiveIDIn / 2;

  /* compute normal and UV coordinates */
  if (faceID < 2) {
    gno = vec3(0., 0., 1.);
  } else if (faceID < 4) {
    gno = vec3(0., 1., 0.);
  } else {
    gno = vec3(1., 0., 0.);
  }
  if ((faceID & 1) == 1) {
    gno = -gno;
  }

  /* emit the vertices */
  for (int i = 0; i < 3; ++i)
    emit(i);
}
