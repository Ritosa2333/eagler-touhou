#pragma once
#include <array>
#include <cmath>
namespace th12::eb {
// Native40c480 uses the raw sprite rectangle (+34 height/+38 width), before
// the ANM scale/rotation transforms. A caller supplies the currently bound
// sprite, so a transform or sprite change affects this gate synchronously.
struct BulletGateGeometry {
  float x=0,y=0,angle=0,spriteWidth=0,spriteHeight=0;
};
// Native40c480's kind400 (decimal1024). Unlike1000 it decrements before
// testing completion; d!=0 also ends an outside bullet's wait when its ray
// cannot reach the sprite-expanded playfield. This bit protects lifetime.
struct BulletGate {
  int remaining = 0;
  float value = 0;
  void begin(int duration) {
    remaining = duration;
    value = float(duration);
  }
  bool tick(float gameRate = 1) {
    value = float(double(value) - gameRate);
    remaining = int(value);
    return remaining <= 0;
  }
  static bool geometryFinished(const BulletGateGeometry& g) {
    // Native409970: edge contact counts as outside, with x87 comparisons
    // before rounding the corner vectors to float32.
    const double halfWidth=double(g.spriteWidth)*.5,halfHeight=double(g.spriteHeight)*.5;
    const bool outside=double(g.x)+halfWidth<=-192 || double(g.x)-halfWidth>=192 ||
                       double(g.y)+halfHeight<=0 || double(g.y)-halfHeight>=448;
    if(!outside)return false;
    const float cosine=float(std::cos(double(g.angle))),sine=float(std::sin(double(g.angle)));
    const std::array<std::array<float,2>,4> corners={{{
      float((-384.0-g.spriteWidth)*.5-g.x),float((-448.0-g.spriteHeight)*.5+224-g.y)}, {
      float((384.0+g.spriteWidth)*.5-g.x),float((-448.0-g.spriteHeight)*.5+224-g.y)}, {
      float((-384.0-g.spriteWidth)*.5-g.x),float((448.0+g.spriteHeight)*.5+224-g.y)}, {
      float((384.0+g.spriteWidth)*.5-g.x),float((448.0+g.spriteHeight)*.5+224-g.y)}}};
    float nonPositiveSide=-999,positiveSide=-999;
    for(const auto& corner:corners){
      // Actual D3DX9_40 SSE2 normalization tests the float32 sum of two
      // rounded squares against2^-46, then uses rsqrt/Newton. Portable sqrt
      // retains that threshold and the independently verified gate behavior;
      // no CPU-specific reciprocal-square-root byte equality is claimed.
      const float squared=float(float(corner[0]*corner[0])+float(corner[1]*corner[1]));
      float nx=0,ny=0;
      if(!(squared<0x1p-46f)){
        const float reciprocalLength=float(1.0/std::sqrt(double(squared)));
        nx=float(corner[0]*reciprocalLength);ny=float(corner[1]*reciprocalLength);
      }
      const float cross=float(double(cosine)*ny-double(sine)*nx);
      const float dot=float(double(cosine)*nx+double(sine)*ny);
      if(dot<0)continue;
      // A zero cross belongs to both sides: a forward ray touching a corner
      // still reaches the expanded rectangle (Native40c6dc..40c85e).
      if(cross<=0&&dot>nonPositiveSide)nonPositiveSide=dot;
      if(cross>=0&&dot>positiveSide)positiveSide=dot;
    }
    // Native40c85e: a side still holding the sentinel has no forward corner.
    return nonPositiveSide<-998 || positiveSide<-998;
  }
  bool tick(const BulletGateGeometry& geometry,bool useGeometry,float gameRate=1) {
    const bool timerFinished=tick(gameRate);
    return (useGeometry&&geometryFinished(geometry))||timerFinished;
  }
};
} // namespace th12::eb
