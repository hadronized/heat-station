#version 330 core

layout (location = 0) in vec3 co;

out vec3 vco;

void main() {
  vco = co;
}
