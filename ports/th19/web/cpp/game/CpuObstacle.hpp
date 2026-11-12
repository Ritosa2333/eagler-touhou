#pragma once
#include "CpuAvoidance.hpp"
#include <optional>
#include <vector>
#include <cstdio>
namespace th19 {
// 4f9530 selects an approaching object; 4fa3e0 steers around it before
// the eighteen-candidate collision pass. Coordinates are field-local.
struct CpuObstacle {
 struct Object {CpuAvoidance::Hazard shape;float heading=0,speed=0;bool line=false;};
 static constexpr float pi=3.14159265358979323846f;
 static float wrap(float a){return std::remainder(a,2*pi);}
 static std::optional<int> steer(float x,float y,int original,const std::vector<Object>&objects){
  const Object* selected=nullptr;float closestX=0,closestY=0,best=pi/2;
  for(const auto&o:objects){const auto&h=o.shape;float px=h.x,py=h.y;
   if(o.line){float c=std::cos(h.angle),s=std::sin(h.angle);
    const float dx=x-h.x,dy=y-h.y;
    const float localX=dx*c+dy*s,localY=dy*c-dx*s;
    float along=std::clamp(localX,-h.width/2,h.width/2);px+=along*c;py+=along*s;
    // 0x4d3580 measures in rotated coordinates before rounding the returned
    // world point. 0x4f97aa then squares that rounded distance again.
    const float ex=along-localX;
    const float distance=float(std::sqrt(double(ex*ex+localY*localY)));
    if(distance*distance-h.height*h.height*o.speed>32*32)continue;
   }else{
    float dx=x-px,dy=y-py,d=dx*dx+dy*dy-h.radius*h.radius*o.speed;
    if(d>=128*128)continue;
    float delta=std::abs(wrap(o.heading-std::atan2(dy,dx)));
    if(d>32*32){
     // 4d3720: forward ray / circle intersection, radius +16.
     float along=dx*std::cos(o.heading)+dy*std::sin(o.heading);
     float across=-dx*std::sin(o.heading)+dy*std::cos(o.heading),r=h.radius+16;
     if(delta>=pi/6||delta>=best||std::abs(across)>r||along< -r||(along<0&&dx*dx+dy*dy>r*r))continue;
     best=delta;
    }else if(delta>=pi/2)continue;
   }
   extern bool traceOpeningRng;
   if(traceOpeningRng)std::printf("ENEMYDBG {\"event\":\"obstacle-choice\",\"index\":%d,\"player\":[%.9g,%.9g],\"closest\":[%.9g,%.9g]}\n",int(&o-objects.data()),x,y,px,py);
   selected=&o;closestX=px;closestY=py;
  }
  if(!selected)return original;
  float a=std::atan2(y-selected->shape.y,x-selected->shape.x);
  float dx=x-closestX,dy=y-closestY,r=selected->shape.radius;
  if(dx*dx+dy*dy>=r*r+32*32){float delta=wrap(selected->heading-a);
   // 0x4facab forces neutral and skips the boundary adjustment.
   if(std::abs(delta)>=pi/9)return std::nullopt;
   a=wrap(a+(delta<=0?pi/2:-pi/2));
  }
  // Native edge rotations, before octant quantization (pi/8 boundaries).
  if(x< -128){if(a<=-7*pi/8)a=wrap(a+pi/2);else if(a>=7*pi/8)a=wrap(a-pi/2);}
  else if(x>128){if(a>=-pi/8&&a<=0)a=wrap(a-pi/2);else if(a>=0&&a<=pi/8)a=wrap(a+pi/2);}
  if(y<128){if(a>=-5*pi/8&&a<=-4*pi/8)a=wrap(a-pi/2);else if(a>=-4*pi/8&&a<=-3*pi/8)a=wrap(a+pi/2);}
  else if(y>416){if(a>=3*pi/8&&a<=4*pi/8)a=wrap(a-pi/2);else if(a>=4*pi/8&&a<=5*pi/8)a=wrap(a+pi/2);}
  if(a<=-7*pi/8||a>=7*pi/8)return 3;
  if(a<=-5*pi/8)return 5;if(a<=-3*pi/8)return 1;if(a<=-pi/8)return 6;
  if(a<=pi/8)return 4;if(a<=3*pi/8)return 8;if(a<=5*pi/8)return 2;return 7;
 }
};
}
