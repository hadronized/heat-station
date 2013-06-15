#version 330 core

out vec4 frag;

uniform vec4 res;

vec2 rand2(vec2 co) {
  return (vec2(fract(sin(dot(co.xy, vec2(12.9898, 78.233)*(1.+seed))) * 43858.5453),
               fract(sin(dot(-co.xy, vec2(-78.8765, 0.764)*(2.+seed))) * 34890.8524)) - .5) * 2.;
}

