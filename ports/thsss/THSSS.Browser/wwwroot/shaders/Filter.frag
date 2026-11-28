#version 300 es
precision mediump float;
uniform sampler2D uTex;
uniform float uFlipV;
in vec2 vUv;
in vec4 vColor;
out vec4 o;
void main() {
  vec2 uv = vec2(vUv.x, mix(vUv.y, 1.0 - vUv.y, uFlipV));
  vec4 Color = texture(uTex, uv);
  if (uFlipV > 0.5) Color.a = 1.0;
  o = Color * vColor;
}
