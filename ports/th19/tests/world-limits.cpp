#include "GameWorld.hpp"
#include "WorldCheckpoint.hpp"
#include "ReplayInput.hpp"
#include <cassert>
#include <iostream>
int main(){
 th19::ReplayInput replay;bool physical[512]{},restored[512]{};th19::u8 injected[512]{};
 physical[29]=true;replay.begin();for(int i=0;i<120;i++)replay.capture(physical,injected,0,0,false);
 assert(replay.frames==120&&replay.records.size()==1&&replay.records[0].ticks==120);
 replay.transfer.resize(80);std::memcpy(replay.transfer.data(),replay.records.data(),80);assert(replay.load(80));
 float x=0,y=0;bool direct=false;for(int i=0;i<120;i++){assert(replay.next(restored,injected,x,y,direct));assert(restored[29]);}
 assert(!replay.next(restored,injected,x,y,direct));
 replay.transfer[76]=2;assert(!replay.load(80));
 th19::GameWorld w;th19::LaserShooter shot;shot.type=0;shot.count1=64;shot.count2=64;
 shot.speed=shot.speed2=0;
 assert(w.fire_bullets(shot,0,100));
 assert(w.bullets.size()==th19::GameWorld::bullet_capacity);
 assert(w.dropped_bullets==4096-th19::GameWorld::bullet_capacity);
 for(int i=0;i<30;++i)assert(w.fire_bullets(shot,0,100));
 assert(w.bullets.size()==th19::GameWorld::bullet_capacity);
 for(auto&b:w.bullets)b.state=4;
 w.tick_wave();assert(w.bullets.empty());
 assert(w.fire_bullets(shot,0,100));assert(w.bullets.size()==th19::GameWorld::bullet_capacity);
 for(int wave=0;wave<30;++wave){
  for(int i=0;i<500;++i){auto&e=w.spawn_enemy("",0,0,1,0,0);e.active=false;}
  w.tick_wave();assert(w.enemies.empty());
 }
 th19::GameWorld right;th19::EclVm leftVm,rightVm;
 th19::AnmVm::restore_random(123);th19::AnmVm::restore_random(456,true);
 th19::WorldCheckpoint snapshot(w,leftVm,right,rightVm);
 th19::AnmVm draw;draw.read_var_f(10011);draw.read_var_f(10031);
 w.bullets[0].x=90;snapshot.restore(w,leftVm,right,rightVm);
 assert(th19::AnmVm::random_checkpoint()==123&&th19::AnmVm::random_checkpoint(true)==456);
 th19::AnmVm::restore_random(789);th19::AnmVm::restore_random(987,true);
 snapshot.capture(w,leftVm,right,rightVm);draw.read_var_f(10011);draw.read_var_f(10031);
 snapshot.restore(w,leftVm,right,rightVm);
 assert(th19::AnmVm::random_checkpoint()==789&&th19::AnmVm::random_checkpoint(true)==987);
 assert(w.bullets[0].x!=90);assert(w.bullets.size()==2000);
 std::cout<<"2000 bullet slots, saturation, recycling, 15000 retired enemies and checkpoint restore: PASS\n";
}
