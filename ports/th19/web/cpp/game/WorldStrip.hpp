#pragma once
#include "Types.hpp"
#include <array>
#include <algorithm>
#include <cmath>
namespace th19 {
// Original callbacks 5/6: 0x4dc510/0x4dc6d0 and 0x4dafd0/0x4db170.
// Local deterministic RNG until the native shared RNG stream is implemented.
struct WorldStrip {
 int type=0,frame=1;u32 rng=1;float angle=.1745329252f;
 std::array<Vec3,40> steps{},points{};
 void init(int effect,u32 seed){type=effect;frame=1;rng=seed?seed:1;angle=.1745329252f;
  for(auto&v:steps)v=type==5?Vec3{0,0,4}:Vec3{-3,0,-16};build();}
 float random(){rng=rng*1664525u+1013904223u;return float(rng>>8)/16777216.f;}
 int count()const{return type==5?40:16;}
 void tick(){
  if((type==5&&(frame&1)==0)||(type==6&&frame%4==0)){
   for(int i=count()-1;i>0;--i)steps[i]=steps[i-1];
   if(type==5){float r=random();angle+=(angle>=.5235987756f?-r:angle<=.1745329252f?r:(r*2-1)*2)*.01745329252f;
    steps[0]={-4*std::sin(angle),0,4*std::cos(angle)};
   }else{float r=random();steps[0].x+=(steps[0].x>=3?-r:steps[0].x<=0?r:r*2-1)*.1f;}
  }
  build();++frame;
 }
 void build(){Vec3 p{0,0,type==5?-200.f:0.f};for(int i=0;i<count();++i){points[i]=p;p.x+=steps[i].x;p.y+=steps[i].y;p.z+=steps[i].z;}}
 u32 color(int row,int side)const{
  if(type==5)return 0x80ffffff;
  // Vertex diffuse words written by 0x4db170 (six floats per vertex).
  int word=row*12+side*6+3;
  switch(word){
   case 0xb7:case 0xbd:case 3:case 9:case 0x5d:case 0x3f:
   case 0x93:case 0x15:case 0x4b:case 0xa5:return 0x00ffffff;
   case 0xab:case 0xb1:return 0x50ffffff;
   case 0x9f:return 0xb0ffffff;
   default:return 0x80ffffff;
  }
 }
 float width(int row)const{return type==5?8.f:std::min(8.f,row*2.f);}
};
}
