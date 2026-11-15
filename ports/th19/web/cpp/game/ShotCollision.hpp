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
