#pragma once
#include "AnmVm.hpp"
#include <cmath>
#include <algorithm>
namespace th19 {
struct AnmGeometryVertex {float x,y;u32 color;};
// 0x4c4060 and 0x4c3b60 draw screen-plane circles.
// 615 dispatch at 0x4c5be0 adds half the width BEFORE the ring helper: [R,R+W].
inline std::vector<AnmGeometryVertex> anmGeometry(const AnmVm&v,float sx=1,float sy=1,float secondaryParent=-1){
 std::vector<AnmGeometryVertex> out;if(!v.visible)return out;
 if(v.geometry==0)return out;
 out.reserve(std::clamp(v.segments,4,256)*6);
 const u32 c=(u32(v.alpha)<<24)|(u32(v.r)<<16)|(u32(v.g)<<8)|v.b;
 const u32 d=(u32(v.alpha2)<<24)|(u32(v.r2)<<16)|(u32(v.g2)<<8)|v.b2;
 const float parentY=secondaryParent<0?sy:secondaryParent;
 sx*=v.sx;sy*=v.sy;
 auto tri=[&](AnmGeometryVertex a,AnmGeometryVertex b,AnmGeometryVertex c){out.push_back(a);out.push_back(b);out.push_back(c);};
 if(v.geometry==626||v.geometry==627){
  // 4c2460 with fixed corner/stroke dimensions; only straight spans scale.
  const int n=std::clamp(v.segments,4,256);const float corner=v.geometry_corner,stroke=v.geometry_stroke;
  const float w=v.geometry_width*sx-2*corner,h=v.geometry_height*sy-2*corner;
  auto point=[&](int j,bool outside){int k=j==n?0:j*4/n;float t=-3.141592654f+j*6.28318530718f/n;
   const float r=corner+(v.geometry==626?(outside?stroke:0):(outside?0:-stroke));
   float x=std::cos(t)*r+(k==0||k==3?-1:1)*w*.5f,y=std::sin(t)*r+(k<2?-1:1)*h*.5f;
   return AnmGeometryVertex{x*std::cos(v.rot_z)-y*std::sin(v.rot_z),x*std::sin(v.rot_z)+y*std::cos(v.rot_z),outside?d:c};};
  for(int j=0;j<n;++j){auto a=point(j,false),b=point(j,true),c=point(j+1,false),d=point(j+1,true);tri(a,b,c);tri(b,d,c);}
 }else if(v.geometry==621||v.geometry==622||v.geometry==631||v.geometry==632){
  // Native 4c3170 / 4c1d90: paired inner/outer contour strips.
  const int n=std::clamp(v.segments,3,256);const float w=v.geometry_width*sy;
  const bool star=v.geometry==631||v.geometry==632,outward=v.geometry==621||v.geometry==631;
  auto point=[&](int j,bool outside){
   float t=j*6.28318530718f/n,delta=outward?(outside?w:0):(outside?0:-w);
   float a=v.geometry_radius*sx,b=v.secondary_sx*parentY;
   float x,y;
   if(star){float r=(j%2?b:a)+delta;x=std::cos(t)*r;y=std::sin(t)*r;}
   else{x=std::cos(t)*(b+delta);y=std::sin(t)*(a+delta);}
   return AnmGeometryVertex{x*std::cos(v.rot_z)-y*std::sin(v.rot_z),x*std::sin(v.rot_z)+y*std::cos(v.rot_z),outside?d:c};
  };
  for(int j=0;j<n;++j){auto a=point(j,false),b=point(j,true),c=point(j+1,false),d=point(j+1,true);tri(a,b,c);tri(b,d,c);}
 }else if(v.geometry==604||v.geometry==615||v.geometry==616||v.geometry==611||v.geometry==617||v.geometry==628||v.geometry==623){
  const int n=std::clamp(v.segments,3,256);const float radius=v.geometry_radius*sx,width=v.geometry_width*sy;
  for(int i=0;i<n;++i){float a=v.rot_z+i*6.283185307f/n,b=v.rot_z+(i+1)*6.283185307f/n;
   auto point=[](float a,float r,u32 col){return AnmGeometryVertex{std::cos(a)*r,std::sin(a)*r,col};};
   if(v.geometry==604)tri({0,0,c},point(a,radius,d),point(b,radius,d));
   else if(v.geometry==615||v.geometry==616||v.geometry==611){float inner=v.geometry==615?radius:v.geometry==616?radius-width:radius-width*.5f,outer=v.geometry==615?radius+width:v.geometry==616?radius:radius+width*.5f;auto p=point(a,inner,c),q=point(a,outer,d),r=point(b,inner,c),s=point(b,outer,d);tri(p,q,r);tri(q,s,r);}
   else {
    auto edge=[&](int j){float t=j*6.283185307f/n,px=0,py=0;
     // 0x4c585e -> 0x4c3850: secondary scale is the local X
     // semiaxis (XMM3), primary radius is local Y (stack argument).
     // 0x404f10 multiplies +0x5c through the parent chain only; it
     // does NOT multiply this axis by the VM's primary Y scale (+0x58).
     if(v.geometry==617){px=std::cos(t)*v.geometry_radius*v.secondary_sx*parentY;py=std::sin(t)*radius;}
     else if(v.geometry==628){float rad=(j%2)?v.secondary_sx*parentY:radius;px=std::cos(t)*rad;py=std::sin(t)*rad;}
     else {t-=3.141592654f;float corner=v.geometry_corner;px=std::cos(t)*corner;py=std::sin(t)*corner;float w=v.geometry_width*sx-2*corner,h=v.geometry_height*sy-2*corner;
      int k=j==n?0:j*4/n;px+=(k==0||k==3?-1:1)*w*.5f;py+=(k<2?-1:1)*h*.5f;}
     return AnmGeometryVertex{px*std::cos(v.rot_z)-py*std::sin(v.rot_z),px*std::sin(v.rot_z)+py*std::cos(v.rot_z),d};};
    tri({0,0,c},edge(i),edge(i+1));
   }
  }
 }else if(v.geometry==605){
  // 0x4c5570: D3DPT_LINESTRIP, one-pixel stroke, constant primary color.
  // Expand each segment for the triangle-only web renderer; no filled disk.
  const int n=std::clamp(v.segments,3,256);const float radius=v.geometry_radius*sx;
  for(int i=0;i<n;++i){float a=v.rot_z+i*6.283185307f/n,b=v.rot_z+(i+1)*6.283185307f/n;
   float ax=std::cos(a)*radius,ay=std::sin(a)*radius,bx=std::cos(b)*radius,by=std::sin(b)*radius;
   float dx=bx-ax,dy=by-ay,len=std::sqrt(dx*dx+dy*dy);if(len<=0)continue;
   float nx=-dy*.5f/len,ny=dx*.5f/len;
   AnmGeometryVertex p{ax+nx,ay+ny,c},q{ax-nx,ay-ny,c},r{bx+nx,by+ny,c},t{bx-nx,by-ny,c};
   tri(p,q,r);tri(q,t,r);
  }
 }else if(v.geometry==618){
  // Native 0x4c50f0 mode 0x20: one XYZRHW/diffuse point, no VM scale.
  tri({-.5f,-.5f,c},{.5f,-.5f,c},{-.5f,.5f,c});
  tri({.5f,-.5f,c},{.5f,.5f,c},{-.5f,.5f,c});
 }else if(v.geometry==614){
  // Native 0x4c4380: right-facing triangle, centroid anchor along X.
  const float w=v.geometry_width*sx,h=v.geometry_height*sy;
  const float left=v.anchor_x==1?0:v.anchor_x==2?-w:-w/3;
  const float right=v.anchor_x==1?w:v.anchor_x==2?0:2*w/3;
  const float top=v.anchor_y==1?0:v.anchor_y==2?-h:-h/2;
  const float bottom=v.anchor_y==1?h:v.anchor_y==2?0:h/2;
  auto point=[&](float x,float y,u32 col){return AnmGeometryVertex{x*std::cos(v.rot_z)-y*std::sin(v.rot_z),x*std::sin(v.rot_z)+y*std::cos(v.rot_z),col};};
  tri(point(left,top,c),point(right,(top+bottom)/2,d),point(left,bottom,c));
 }else if(v.geometry==608){
  // 4c42b0 calls 4c4d20 twice: W+1/H+1 at half alpha, then W/H.
  // Both use the same rotation and horizontal primary/secondary colors.
  for(int pass=0;pass<2;++pass){
   const float w=v.geometry_width*sx+(pass?0:1),h=v.geometry_height*sy+(pass?0:1);
   const float x=v.anchor_x==1?0:v.anchor_x==2?-w:-w*.5f,y=v.anchor_y==1?0:v.anchor_y==2?-h:-h*.5f;
   auto point=[&](float px,float py,u32 col){if(!pass)col=(col&0xffffff)|((col>>1)&0x7f000000);return AnmGeometryVertex{px*std::cos(v.rot_z)-py*std::sin(v.rot_z),px*std::sin(v.rot_z)+py*std::cos(v.rot_z),col};};
   auto p=point(x,y,c),q=point(x+w,y,d),r=point(x,y+h,c),t=point(x+w,y+h,d);tri(p,q,r);tri(q,t,r);
  }
 }else if(v.geometry==603||v.geometry==606){
  const float w=v.geometry_width*sx,h=v.geometry_height*sy;
  const float x=v.anchor_x==1?0:v.anchor_x==2?-w:-w*.5f,y=v.anchor_y==1?0:v.anchor_y==2?-h:-h*.5f;
  auto point=[&](float px,float py,u32 col){return AnmGeometryVertex{px*std::cos(v.rot_z)-py*std::sin(v.rot_z),px*std::sin(v.rot_z)+py*std::cos(v.rot_z),col};};
  auto p=point(x,y,c),q=point(x+w,y,v.geometry==603?c:d),r=point(x,y+h,c),s=point(x+w,y+h,v.geometry==603?c:d);tri(p,q,r);tri(q,s,r);
 }
 return out;
}
}
