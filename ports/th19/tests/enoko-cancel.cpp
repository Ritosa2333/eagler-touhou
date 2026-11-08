#include "GameWorld.hpp"
#include "PlayerShotTable.hpp"
#include <cassert>
#include <iostream>
int main(){
 using namespace th19;
 for(int age=0;age<100;++age)assert(enokoChargeCancels(age,30)==(age>=40&&age<50));
 // Every native ordinary descriptor: no style/color whitelist.
 for(int kind=0;kind<39;++kind)for(int state=0;state<=4;++state)for(int lock=0;lock<=1;++lock){
  GameWorld w;auto&b=w.spawn_bullet();b.x=0;b.y=100;b.kind=kind;b.state=state;b.cancel_lock=lock;b.hit_width=2;
  w.cancel_enoko_trap(0,100,0);
  const bool allowed=(state==1||state==2)&&lock==0;
  assert(w.cancelled_bullets==int(allowed));assert(w.bullets[0].state==(allowed?4:state));
 }
 for(float angle:{0.f,1.57079632679f}){GameWorld w;
  auto&b=w.spawn_bullet();b.x=20;b.y=100;b.state=1;b.hit_width=0;
  w.cancel_enoko_trap(0,100,angle);assert(w.cancelled_bullets==int(angle!=0));
 }
 {GameWorld w;auto&b=w.spawn_bullet();b.x=200;b.y=100;b.state=1;b.hit_width=2;
  w.cancel_enoko_trap(200,100,0);assert(w.cancelled_bullets==0);}
 {GameWorld w;auto&b=w.spawn_bullet();b.x=149;b.y=100;b.state=1;b.hit_width=2;
  w.cancel_enoko_trap(149,100,0);assert(w.cancelled_bullets==1);}
 for(bool straight:{false,true})for(int type:{0,8,18,31,38})for(int lock:{0,1}){
  GameWorld w;InfiniteLaser l;l.x=-24;l.y=100;l.angle=0;l.length=48;l.phase=2;
  l.straight=straight;l.source.type=type;l.cancel_lock=lock;w.lasers.push_back(l);
  w.cancel_enoko_trap(0,100,0);assert((w.cancelled_bullets>0)==!lock);
 }
 for(int lock:{0,1}){GameWorld w;InfiniteLaser l;l.curved=true;l.cancel_lock=lock;l.phase=2;
  for(int i=0;i<8;++i)l.curve.nodes.push_back({float(i),100});w.lasers.push_back(l);
  w.cancel_enoko_trap(0,100,0);assert(w.cancelled_bullets==(lock?0:8));}
 std::cout<<"PASS: Enoko bite window, 390 state/style/lock combinations, rotated area, viewport and laser locks\n";
}
