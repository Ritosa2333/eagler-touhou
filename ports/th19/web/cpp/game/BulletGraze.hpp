#pragma once
#include <algorithm>
#include <cmath>
namespace th19 {
// 0x549e00 circular branch: hit and graze use distinct strict squared boundaries.
inline int nativeCircularBulletContact(float dx,float dy,float playerRadius,float bulletRadius){
 if(bulletRadius<=0)return 0;
 const float d=dx*dx+dy*dy;
 if(d<playerRadius*playerRadius+bulletRadius*bulletRadius)return 1;
 const float r=playerRadius+std::max(40.f,bulletRadius/2.5f);
 return d<r*r+bulletRadius*bulletRadius?2:0;
}
// 0x549bd0: inclusive AABB overlap; graze uses a fixed 48x48 box.
// Native player initialization 0x20cc/20d0 and focused20d8/20dc = 3/2.
inline int nativeRectangularBulletContact(float dx,float dy,float playerHalfWidth,float playerHalfHeight,float width,float height){
 if(width<=0||height<=0)return 0;
 if(std::abs(dx)<=playerHalfWidth+width*.5f&&std::abs(dy)<=playerHalfHeight+height*.5f)return 1;
 return std::abs(dx)<=playerHalfWidth+24&&std::abs(dy)<=playerHalfHeight+24?2:0;
}
inline int nativeOrdinaryBulletContact(int kind,float dx,float dy,float radius,float playerRadius=3){
 return kind==17?nativeRectangularBulletContact(dx,dy,1.5f,1.5f,radius,radius):nativeCircularBulletContact(dx,dy,playerRadius,radius);
}
// Bullet timers fd8/fec/1000, interval1014, stock1018; 4f5130/4f54a7.
struct BulletGraze {
 int elapsed=0,previous=-1,contactFrames=0,outsideFrames=0,stock=1;
 int interval=120,capacity=1;
 struct Result {bool reward=false;int interrupt=0;int alpha=255;};
 Result tick(bool contact){
  Result r;
  if(contact){
   if(stock>0&&elapsed!=previous&&elapsed%interval==0){r.reward=true;elapsed=0;previous=-1;--stock;}
   previous=elapsed++;++contactFrames;outsideFrames=0;
   if(contactFrames==1)r.interrupt=2;
   r.alpha=std::clamp((104-contactFrames)*2,96,255);
  }else{
   ++outsideFrames;
   if(outsideFrames>=60&&stock==0){stock=capacity;previous=-1;elapsed=0;}
   if(contactFrames)r.interrupt=3;
   contactFrames=0;
  }
  return r;
 }
};
}
