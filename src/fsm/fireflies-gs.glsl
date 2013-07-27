#version 330 core

layout (points) in;
layout (triangles_strip, max_vertices = 6) out;

in vec3 vco;

uniform mat4 proj;
uniform mat4 view;

const float s = 0.25;

const vec2[4] tile = vec2[](
    vec2(-s,  s)
  , vec2( s,  s)
  , vec2( s, -s)
  , vec2(-s, -s)
);

void emit_vertex(uint i) {
  gl_Position = proj * view * vec4(vco+vec3(tile[i], 0.), 1.);
  EmitVertex();
}

void main() {
  for (uint i = 0u; i < 4u; ++i)
    emit_vertex(i);
}
