#version 330

out vec4 frag;

uniform vec4 res;

const float seed = 0.;
vec2 rand2(vec2 co) {
  return (vec2(fract(sin(dot(co.xy, vec2(12.9898, 78.233)*(1.+seed))) * 43858.5453),
        fract(sin(dot(-co.xy, vec2(-78.8765, 0.764)*(2.+seed))) * 34890.8524)) - .5) * 2.;
}

float f(float x) {
  return 6.*pow(x, 5.) - 15.*pow(x, 4.) + 10.*pow(x, 3.);
}

float perlin_noise(vec2 uv) {
  vec2 uv00 = floor(uv); /* get the lower left corner of the cell the point is in */
  vec2 uv10 = uv00 + vec2(1., 0.);
  vec2 uv01 = uv00 + vec2(0., 1.);
  vec2 uv11 = uv00 + vec2(1., 1.);
  vec2 g00  = normalize(rand2(uv00));
  vec2 g10  = normalize(rand2(uv10));
  vec2 g01  = normalize(rand2(uv01));
  vec2 g11  = normalize(rand2(uv11));
  vec2 p00 = fract(uv); /* uv - uv00; */
  vec2 p10 = p00 - vec2(1., 0);
  vec2 p01 = p00 - vec2(0., 1.);
  vec2 p11 = p00 - vec2(1., 1.);

  /*
     float perlinX0 = mix(dot(g00, p00), dot(g10, p10), p00.x);
     float perlinX1 = mix(dot(g01, p01), dot(g11, p11), p00.x);
     float perlin   = mix(perlinX0, perlinX1, p00.y);
   */
  float perlinX0 = dot(g00, p00)*(1. - f(p00.x)) + dot(g10, p10)*f(p00.x);
  float perlinX1 = dot(g01, p01)*(1. - f(p00.x)) + dot(g11, p11)*f(p00.x);
  float perlin   = perlinX0*(1. - f(p00.y)) + perlinX1*f(p00.y);

  return perlin;
}

vec2 get_uv() {
  vec2 uv = gl_FragCoord.xy * res.zw;
  //uv.y *= res.y * res.z;
  return uv;
}

void main() {
  vec2 uv = get_uv();

  frag = vec4(0.);
  float pers = 0.5;
#if 0
  for (int i = 0; i < 2; i++) {
    frag += vec4(perlin_noise(uv*pow(2., float(i+2))) * pow(pers, float(i+0)));
  }
#endif

  frag += vec4(perlin_noise(uv*pow(2., 5.5)) * pow(pers, .25));
  frag += vec4(perlin_noise(uv*pow(2., 6.)) * pow(pers, .5));
  frag += vec4(perlin_noise(uv*pow(2., 6.5)) * pow(pers, .75));
  frag += vec4(perlin_noise(uv*pow(2., 7.)) * pow(pers, 1.));

  frag += 1.;
  frag *= 0.5;
}
