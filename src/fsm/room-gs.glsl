#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 proj;
uniform mat4 view;

in vec3 rco[];
in vec3 vco[];

out vec3 gco;
out vec3 gno;

uniform float size;

void emit(int i) {
  /* coordinates */
  gco = vco[i];
  gl_Position = proj * view * vec4(vco[i], 1.);

  EmitVertex();
}

void main() {
  /* compute normal */
  if (gl_PrimitiveIDIn < 4) {
    gno = vec3(0., 0., 1.);
  } else if (gl_PrimitiveIDIn < 8) {
    gno = vec3(0., 1., 0.);
  } else {
    gno = vec3(1., 0., 0.);
  }
  if (gl_PrimitiveIDIn / 2 == 1.)
    gno = -gno;

  /* emit the vertices */
  for (int i = 0; i < 3; ++i)
    emit(i);
}
