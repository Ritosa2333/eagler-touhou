#pragma once
#include <cmath>
#include <algorithm>
namespace th19 {
// 4d26a0 moves the contact point toward the polygon centre by the
// bullet's hit radius, then 4d2a10 tests it against every polygon edge.
inline bool nativePolygonContact(float dx,float dy,float radius,float angle,int sides,float hitRadius=0){
 const float d=std::hypot(dx,dy);if(d<=hitRadius)return true;
 dx*=1-hitRadius/d;dy*=1-hitRadius/d;
 for(int i=0;i<sides;++i){
  const float a=angle+6.28318530718f*i/sides,b=angle+6.28318530718f*(i+1)/sides;
  const float ax=radius*std::cos(a),ay=radius*std::sin(a),bx=radius*std::cos(b),by=radius*std::sin(b);
  if((bx-ax)*(dy-ay)-(by-ay)*(dx-ax)<0)return false;
 }return true;
}
// 4d1cc0 / 4d2070: radial contact point and alternating star vertices.
inline bool nativeStarContact(float dx,float dy,float outer,float inner,float angle,float hitRadius=0){
 float d=std::hypot(dx,dy);if(d<=hitRadius)return true;dx*=1-hitRadius/d;dy*=1-hitRadius/d;
 // 4d2070 rejects on the first edge-side failure, even for a concave star.
 for(int i=0;i<10;++i){int j=(i+1)%10;
  float ai=angle+i*.628318530718f,aj=angle+j*.628318530718f;
  float ri=i%2?inner:outer,rj=j%2?inner:outer;
  float ix=std::cos(ai)*ri,iy=std::sin(ai)*ri,jx=std::cos(aj)*rj,jy=std::sin(aj)*rj;
  if((jx-ix)*(dy-iy)-(jy-iy)*(dx-ix)<0)return false;
 }return true;
}
// 4d4020: eroded-ellipse early acceptance, then native discrete contact samples.
inline bool nativeEllipseContact(float dx,float dy,float major,float minor,float angle,float hitRadius=0){
 const float c=std::cos(angle),s=std::sin(angle),u=dx*c+dy*s,v=-dx*s+dy*c;
 const float a=minor,b=major,r=hitRadius;
 if(r<std::min(a,b)&&u*u/((a-r)*(a-r))+v*v/((b-r)*(b-r))<=1)return true;
 const float big=std::max(a,b);
 if(r>big&&(r-big)*(r-big)>=u*u+v*v)return true;
 if(r<1||big<1)return false;
 const int n=std::max(8,int(r<=big?r/4:(a+b)/4));
 for(int i=0;i<n;++i){float t=-3.141592654f+i*6.28318530718f/n;
  if(r<=big){float x=u+r*std::cos(t),y=v+r*std::sin(t);if(x*x/(a*a)+y*y/(b*b)<=1)return true;}
  else{float x=u-a*std::cos(t),y=v-b*std::sin(t);if(x*x+y*y<=r*r)return true;}
 }return false;
}

}
