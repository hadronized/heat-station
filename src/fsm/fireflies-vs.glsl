#version 330 core

layout (location = 0) in vec3 co;
layout (location = 1) in vec3 color;

out vec3 vco;
out vec3 vcolor;

void main() {
  vco = co;
  vcolor = color;
}
