#pragma once
#include <algorithm>
#include <cmath>
namespace th19 {
// The damage area and hit callback have separate clocks. Saki initializer18
// (54d960) sets area+a4=4 even though hit8 (54c7c0) has no modulo gate.
// 51f320 starts the area countdown at 9999999; 51fa30 decrements it before
// 520450 tests the remainder. age0 is the first update after our spawn.
// Marisa's normal hit2 independently gates on the projectile's age (54cd70).
inline bool laserDamageTick(int hitCallback,int age,int initCallback=0){
 if(age<0)return false;
 if(initCallback==18&&(9999999-(age+1))%4!=0)return false;
 return age>0&&(hitCallback!=2||age%4==0);
}
// Normal Marisa laser hit callback 0x54cd70 shortens the beam on contact;
// charge callback 0x54c7c0 does not. Return first contact along an oriented ray.
inline float laserContactDistance(float x,float y,float angle,float length,float width,
                                  float ex,float ey,float ew,float eh){
 const float c=std::cos(angle),s=std::sin(angle);
 const float rx=(ew+std::abs(s)*width)*.5f,ry=(eh+std::abs(c)*width)*.5f;
 float lo=0,hi=length;
 auto axis=[&](float p,float v,float r){if(std::abs(v)<1e-6f)return std::abs(p)<=r;
  float a=(-r-p)/v,b=(r-p)/v;if(a>b)std::swap(a,b);lo=std::max(lo,a);hi=std::min(hi,b);return lo<=hi;};
 return axis(x-ex,c,rx)&&axis(y-ey,s,ry)?lo:-1.f;
}
// 0x54ce3e uses the snapped shot origin for the ray, after the area was
// registered at its unsnapped center. A missed ray writes zero before the +8.
inline float nativeOptionBeamCircleLength(float x,float y,float angle,float ex,float ey,float radius){
    const float dx=ex-std::floor(x*100.f)/100.f;
    const float dy=ey-std::floor(y*100.f)/100.f;
    const float c=std::cos(angle),s=std::sin(angle);
    const float along=dx*c+dy*s,across=dy*c-dx*s;
    if(std::abs(across)>radius||along<-radius||
       (along<0&&along*along+across*across>radius*radius))return 8.f;
    const float offset=across/radius;
    return std::max(0.f,along-std::sqrt(1.f-offset*offset)*radius+8.f);
}
// 0x51e6de -> 0x4d2c30 tests the two current oriented rectangles.
inline bool shotRectanglesOverlap(float dx,float dy,float angle,float width,float height,
                                  float otherAngle,float otherWidth,float otherHeight){
    const float c=std::abs(std::cos(otherAngle-angle));
    const float s=std::abs(std::sin(otherAngle-angle));
    const auto overlaps=[&](float axisAngle,float w,float h,float ow,float oh){
        const float along=dx*std::cos(axisAngle)+dy*std::sin(axisAngle);
        const float across=dy*std::cos(axisAngle)-dx*std::sin(axisAngle);
        return 2*std::abs(along)<w+c*ow+s*oh && 2*std::abs(across)<h+s*ow+c*oh;
    };
    return overlaps(angle,width,height,otherWidth,otherHeight) &&
           overlaps(otherAngle,otherWidth,otherHeight,width,height);
}
// Sweep a shot's oriented hit rectangle against the enemy's hit region.
// Previous/current centers prevent fast SHT shots from tunnelling through enemies.
inline bool sweptShotHit(float x0,float y0,float x1,float y1,float angle,float width,float height,
                         float ex,float ey,float ew,float eh){
 const float c=std::cos(angle),s=std::sin(angle);
 // SHT width runs along the projectile direction; height across it.
 const float rx=(std::abs(c)*width+std::abs(s)*height+ew)*.5f;
 const float ry=(std::abs(s)*width+std::abs(c)*height+eh)*.5f;
 float lo=0,hi=1;
 auto axis=[&](float p,float v,float r){if(std::abs(v)<1e-6f)return std::abs(p)<=r;float a=(-r-p)/v,b=(r-p)/v;if(a>b)std::swap(a,b);lo=std::max(lo,a);hi=std::min(hi,b);return lo<=hi;};
 return axis(x0-ex,x1-x0,rx)&&axis(y0-ey,y1-y0,ry);
}
}
