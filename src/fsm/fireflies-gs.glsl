#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

in vec3 vco[];
in vec3 vcolor[];
out vec2 gco;
flat out vec3 gcolor;

uniform mat4 proj;
uniform mat4 view;

const float s = 0.25;

const vec2[4] tile = vec2[](
    vec2(-s,  s)
  , vec2( s,  s)
  , vec2(-s, -s)
  , vec2( s, -s)
);

void emit_vertex(uint i) {
  gl_Position = proj * (view * vec4(vco[0], 1.) + vec4(tile[i], 0., 1.));
  gco = tile[i] / s;
  gcolor = vcolor[0];
  EmitVertex();
}

void main() {
  for (uint i = 0u; i < 4u; ++i)
    emit_vertex(i);
}
