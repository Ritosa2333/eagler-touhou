#pragma once
#include "AnmVm.hpp"
#include "Types.hpp"
#include <array>
#include <algorithm>
#include <cmath>
namespace th19 {
// Original callbacks 5/6: 0x4dc510/0x4dc6d0 and 0x4dafd0/0x4db170.
struct WorldStrip {
 static constexpr float radiansPerDegree=3.14159274101257324219f/180.f;
 int type=0,frame=0;float angle=10.f*radiansPerDegree;
 std::array<Vec3,40> steps{},points{};
 void init(int effect){type=effect;frame=0;angle=10.f*radiansPerDegree;
  for(auto&v:steps)v=type==5?Vec3{0,0,4}:Vec3{-3,0,-16};build();}
 int count()const{return type==5?40:16;}
 void tick(){
  // The init callbacks sample an angle from the shared ANM stream on the
  // first update. The vertex and draw callbacks do not read that angle.
  if(++frame==1)AnmVm::rand_integer();
  if((type==5&&(frame&1)==0)||(type==6&&frame%4==0)){
   for(int i=count()-1;i>0;--i)steps[i]=steps[i-1];
   if(type==5){
    if(angle>=30.f*radiansPerDegree)angle-=AnmVm::rand_unit()*radiansPerDegree;
    else if(angle<=10.f*radiansPerDegree)angle+=AnmVm::rand_unit()*radiansPerDegree;
    else angle+=AnmVm::rand_signed_unit(radiansPerDegree)*2.f;
    steps[0]={-4*std::sin(angle),0,4*std::cos(angle)};
   }else{
    float& x=steps[0].x;
    if(x>=3.f)x-=AnmVm::rand_unit()*.1f;
    else if(x<=0.f)x+=AnmVm::rand_unit()*.1f;
    else x+=AnmVm::rand_signed_unit(.1f);
   }
  }
  build();
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
