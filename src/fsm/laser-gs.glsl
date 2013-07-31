/*

This geometry shader generates 4 planes from 1 line.
It also generates the texcoord for the fragment shader
texture laser texture lookup.

Output planes:
  hplane: -
  vplane: |
  lplane: \
  rplane: /

*/

#version 330 core

layout (lines) in;
layout (triangle_strip, max_vertices=16) out;

in vec3 vCo[]; /* vertex shader coordinates */

out vec2 gUV; /* geometry shader texture UV */
out float gHeight;

uniform float hheight; /* half height of each plane */
uniform mat4 proj;
uniform mat4 view;

void emit(vec3 a) {
  gl_Position = proj * view * vec4(a, 1.);
  gHeight = a.y;
  EmitVertex();
}

void emit_plane(vec3 a, vec3 b, vec3 c, vec3 d) {
  gUV = vec2(0., 0.);
  emit(a);

  gUV = vec2(1., 0.);
  emit(b);

  gUV = vec2(0., 1.);
  emit(c);

  gUV = vec2(1., 1.);  
  emit(d);

  EndPrimitive();
}

void main() {
  /* hplane */
  vec3 a = vec3(vCo[0].x-hheight, vCo[0].yz); 
  vec3 b = vec3(vCo[0].x+hheight, vCo[0].yz); 
  vec3 c = vec3(vCo[1].x-hheight, vCo[1].yz); 
  vec3 d = vec3(vCo[1].x+hheight, vCo[1].yz); 
  emit_plane(a, b, c, d);

  /* vplane */
  a = vec3(vCo[0].x, vCo[0].y-hheight, vCo[0].z);
  b = vec3(vCo[0].x, vCo[0].y+hheight, vCo[0].z);
  c = vec3(vCo[1].x, vCo[1].y-hheight, vCo[1].z);
  d = vec3(vCo[1].x, vCo[1].y+hheight, vCo[1].z);
  emit_plane(a, b, c, d);

  /* lplane */
  a = vec3(vCo[0].x-hheight, vCo[0].y+hheight, vCo[0].z);
  b = vec3(vCo[0].x+hheight, vCo[0].y-hheight, vCo[0].z);
  c = vec3(vCo[1].x-hheight, vCo[1].y+hheight, vCo[1].z);
  d = vec3(vCo[1].x+hheight, vCo[1].y-hheight, vCo[1].z);
  emit_plane(a, b, c, d);

  /* rplane */
  a = vec3(vCo[0].x-hheight, vCo[0].y-hheight, vCo[0].z);
  b = vec3(vCo[0].x+hheight, vCo[0].y+hheight, vCo[0].z);
  c = vec3(vCo[1].x-hheight, vCo[1].y-hheight, vCo[1].z);
  d = vec3(vCo[1].x+hheight, vCo[1].y+hheight, vCo[1].z);
  emit_plane(a, b, c, d);
}

