#include "../../src/game/PlayerBounds.hpp"
using namespace th12::player_bounds;
extern "C" {
float result[12];float corners[12];
const float* bounds_corners(){return corners;}
const float* bounds_quad(int mode,int ax,int ay,float angle,float x,float y,float w,float h){
  const auto points=vertices({x,y,0,w,h,angle,mode,ax,ay});
  for(int i=0;i<4;i++){result[i*3]=points[i].x;result[i*3+1]=points[i].y;result[i*3+2]=points[i].z;}
  return result;
}
int bounds_survives(int type,int age,float x,float y){std::array<Point,4> p;for(int i=0;i<4;i++)p[i]={corners[i*3],corners[i*3+1],corners[i*3+2]};return survives(type,age,x,y,p);}
int bounds_hit(int type,float y,float h,float ey,float eh){return hitRectangle(0,y,24,h,0,ey,24,eh,type==2);}
}
