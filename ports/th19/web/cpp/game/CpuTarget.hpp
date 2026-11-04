#pragma once
#include <cmath>
#include <cstdint>
#include <vector>
namespace th19 {
// CPUHitReimuInf 0x4f9810: input is the eligible target list gathered by 0x4f9ac0.
// Cooldown is enemy +63ac; status is enemy +63a0. Do not pass ECL flags instead.
struct CpuTarget {
 struct Enemy {uint32_t id=0;float x=0,y=0;uint32_t status=0;int cooldown=0;};
 static bool inRange(float x,float y,const Enemy&e){
  float dx=x-e.x,dy=y-e.y;
  return (dx*dx+dy*dy<=48*48&&y>=e.y+16)||(std::abs(e.x)<100&&dy<256&&y>=e.y-32);
 }
 static int select(float x,float y,std::vector<Enemy>&targets,int character=0){
  int selected=-1;float best=998001;
  for(int i=0;i<int(targets.size());++i){auto&e=targets[i];
   if(e.cooldown>0){--e.cooldown;continue;}
   float dx=x-e.x,dy=y-e.y,d=dx*dx+dy*dy;
   // 0x4f8930: Marisa weights aligned special targets by one third.
   if(character==1&&(e.status&0x700000)&&!(e.status&0x80000)&&std::abs(dx)<64)d/=3;
   bool eligible=(d<=48*48&&y>=e.y+16)||(std::abs(e.x)<100&&dy<256&&y>=e.y-32);
   if(eligible&&d<best&&!((e.status&0x700000)&&(e.status&0x80000))){best=d;selected=i;}
  }
  if(selected>=0)return selected;
  best=998001;
  // Native second pass deliberately ignores cooldown and special exclusion.
  for(int i=0;i<int(targets.size());++i){const auto&e=targets[i];float dx=x-e.x,dy=y-e.y,d=dx*dx+dy*dy;
   if(inRange(x,y,e)&&d<best){best=d;selected=i;}}
  return selected;
 }
 struct Pursuit {int direction=0;bool slow=false,charge=false;int chargeTailFrames=0;bool incrementSpecialCounter=false;};
 // 0x4f8c50 / 0x4f8d10 / 0x4f8dd0. Preserve XMM2/XMM3 arguments,
 // absent from the decompiler prototype: ordinary x=12, y far=256.
 static Pursuit pursue(int character,float x,float y,const Enemy&e){
  Pursuit out;out.slow=x>e.x-64&&x<e.x+64;
  float horizontal=12,near=160,far=256;
  if((e.status&0x700000)&&!(e.status&0x80000)){
   out.charge=true;out.chargeTailFrames=20;out.incrementSpecialCounter=true;
   if(y>128){if(character==1){horizontal=8;near=80;}else{near=16;far=40;}}
  }
  int d=x<e.x-horizontal?4:x>e.x+horizontal?3:0;
  if(y>e.y+far)d=d==4?6:d==3?5:1;
  else if(y<e.y+near)d=d==4?8:d==3?7:2;
  out.direction=d;return out;
 }
};
}
