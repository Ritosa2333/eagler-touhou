#version 300 es
precision mediump float;
uniform sampler2D uTex;
uniform float uFlipV;
uniform float X;
uniform float Y;
uniform float Time;
uniform float Radius;
uniform int ColorKey;
in vec2 vUv;
in vec4 vColor;
out vec4 o;
void main() {
  vec2 tex = vUv;
  float x = X + 20.0 * cos(Time) / 640.0;
  float y = Y + 30.0 * sin(1.5 * Time) / 640.0;
  float deltaX = tex.x - x;
  float deltaY = (tex.y - y) * 3.0 / 4.0;
  float dist2 = deltaX * deltaX + deltaY * deltaY;
  float R = Radius;
  float destX = tex.x;
  float destY = tex.y;
  if (dist2 < R * R) {
    float k = 0.3 * (acos(sqrt(dist2) / R - 1.0) - acos(0.0));
    destX -= deltaX * k;
    destY -= deltaY * k / 3.0 * 4.0;
  }
  x = X + 20.0 * cos(1.5 * Time) / 640.0;
  y = Y + 30.0 * sin(1.0 * Time) / 640.0;
  deltaX = tex.x - x;
  deltaY = (tex.y - y) * 3.0 / 4.0;
  dist2 = deltaX * deltaX + deltaY * deltaY;
  if (dist2 < R * R) {
    float k = 0.3 * (acos(sqrt(dist2) / R - 1.0) - acos(0.0));
    destX -= deltaX * k;
    destY -= deltaY * k / 3.0 * 4.0;
  }
  x = X;
  y = Y;
  deltaX = tex.x - x;
  deltaY = (tex.y - y) * 3.0 / 4.0;
  dist2 = deltaX * deltaX + deltaY * deltaY;
  if (dist2 < R * R) {
    float k = 0.25 * (acos(sqrt(dist2) / R - 1.0) - acos(0.0));
    destX -= deltaX * k;
    destY -= deltaY * k / 3.0 * 4.0;
  }
  vec2 sampleUv = vec2(destX, mix(destY, 1.0 - destY, uFlipV));
  vec4 Color = texture(uTex, sampleUv);
  R += 0.02;
  if (dist2 < R * R) {
    float shade = 10.0 * (R * R - dist2);
    if (ColorKey == 0) Color.r -= shade;
    else if (ColorKey == 1) Color.g -= shade;
    else if (ColorKey == 2) Color.b -= shade;
    else if (ColorKey == 3) { Color.r -= shade; Color.g -= shade; }
    else if (ColorKey == 4) { Color.r -= shade; Color.b -= shade; }
    else if (ColorKey == 5) { Color.b -= shade; Color.g -= shade; }
  }
  if (uFlipV > 0.5) Color.a = 1.0;
  o = Color * vColor;
}
