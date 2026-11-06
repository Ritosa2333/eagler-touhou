#include "GameWorld.hpp"
#include "ChargeGauge.hpp"
#include <fstream>
#include <iostream>
#include <cassert>
using namespace th19;
int main(int argc,char**argv){
 assert(argc==2);
 for(int d=0;d<4;++d){ChargeGauge g;g.initialize_versus_rank(d);assert(g.ex_level==(d<2?0:(d-1)*2));assert(g.boss_level==g.ex_level);}
 ComboAttack combo;
 for(int i=0;i<7;i++)combo.on_death(2,5);
 assert(combo.extraCredit==105&&combo.bossCredit==105);assert(combo.tick()==1);assert(combo.extraCooldown==59);
 for(int i=0;i<7;i++)combo.on_death(2,5);
 assert(combo.tick()==2);assert(combo.bossCooldown==239);
 ComboAttack excluded;excluded.on_death(0,100);excluded.on_death(2,4);assert(excluded.tick()==0);
 std::ifstream file(argv[1],std::ios::binary);std::vector<u8> bytes((std::istreambuf_iterator<char>(file)),{});EclProgram p;assert(p.load(bytes.data(),bytes.size()));
 for(int kind=2;kind<=4;++kind)for(int enhanced=0;enhanced<2;++enhanced){
  GameWorld world;world.attach_program(p);auto arrival=spiritArrival(0,kind,0,0,enhanced,false);assert(world.receive_spirit(arrival,0,50));
  world.tick_wave();auto&e=world.enemies.front();
  const int rank=1+enhanced;const int expected=kind==2?800+rank*200:kind==3?500+rank*200:500+rank*300;
  std::cout<<kind<<" enhanced="<<enhanced<<" hp="<<e.life<<" expected="<<expected<<"\n";assert(e.life==expected);
 }
 
 for(int kind=2;kind<=4;++kind)for(int enhanced=0;enhanced<2;++enhanced){
  GameWorld w;w.attach_program(p);assert(w.receive_spirit(spiritArrival(0,kind,0,0,enhanced,false),0,50));w.tick_wave();
  auto&e=w.enemies.front();e.sprite=0;const int hp=e.life;w.player_damage_cap=90;
  for(int i=0;i<40;++i)w.damage_enemy(e,15);
  assert(e.life==hp-90&&e.active);
  ++w.battle_frame;for(int i=0;i<40;++i)w.damage_enemy(e,15);assert(e.life==hp-180);
  ++w.battle_frame;w.card_near_damage=true;w.player_x=e.x;w.player_y=e.y;
  for(int i=0;i<40;++i)w.damage_enemy(e,15);assert(e.life==hp-270);
  std::cout<<"spirit "<<kind<<" enhanced="<<enhanced<<" overlap and next-frame cap PASS\n";
 }
 std::cout<<"native gameplay assertions PASS\n";
}

