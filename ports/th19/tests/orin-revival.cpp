#include "GameWorld.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
using namespace th19;
int main(int argc,char**argv){
 assert(argc==2);std::ifstream f(argv[1],std::ios::binary);
 std::vector<u8> bytes((std::istreambuf_iterator<char>(f)),{});
 EclProgram p;assert(p.load(bytes.data(),bytes.size()));
 for(int difficulty=0;difficulty<4;++difficulty){
  GameWorld w;w.difficulty=difficulty;w.attach_program(p);
  auto&boss=w.spawn_enemy("",0,128,1000,0,0);const auto bossId=boss.id;
  w.register_boss(boss,0);boss.flags=32;
  auto&fairy=w.spawn_enemy("BossGirl",0,240,600,1,0);const auto id=fairy.id;
  auto get=[&](u32 target)->Enemy&{for(auto&e:w.enemies)if(e.id==target)return e;assert(false);std::abort();};
  auto ticks=[&](int count){for(int n=0;n<count;++n){w.tick_wave();w.finish_hit_feedback(0);}};
  ticks(65);assert(get(id).death_callback=="BossGirlDead");
  assert((get(id).flags&3)==0);assert(w.field_ints[0]==1);
  for(int cycle=0;cycle<2;++cycle){
   get(bossId).ecl_ints[1]=0;
   const int oldDefeated=w.defeated;
   assert(w.damage_enemy(get(id),10000));
   assert(!w.damage_enemy(get(id),10000)); // no double death in the same pass
   w.finish_hit_feedback(0);
   assert(get(id).active&&get(id).life==100&&get(id).sprite==45);
   assert(get(id).death_callback.empty()&&(get(id).flags&3)==3);
   assert(w.field_ints[0]==0&&w.defeated==oldDefeated+1);
   assert(!w.deaths.empty()&&w.deaths.back().script==262);
   ticks(100);assert(get(id).sprite==45); // wait indefinitely for boss signal
   get(bossId).ecl_ints[1]=1;
   ticks(2);assert(get(id).death_callback=="BossGirlDead");
   assert(get(id).animations[2].script==38&&w.field_ints[0]==1);
   ticks(61);assert((get(id).flags&3)==0&&get(id).directional_base==46);
   assert(get(id).id==id); // same host, not a replacement enemy
  }
  assert(w.unhandled_opcodes.empty());
  std::cout<<"difficulty "<<difficulty<<": original BossGirl death/wait/revive twice PASS\n";
 }
}
