#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
using namespace th13;
namespace {
struct Host:StageBattleServices{bool life_hud(i32,i32)override{return true;}bool bomb_hud(i32,i32)override{return true;}bool request_game_over()override{return true;}bool hud_sound(i32)override{return true;}};
struct Provider:EclResourceProvider{
 std::vector<u8> bytes;std::vector<std::string>* names=nullptr;
 Provider(){std::vector<u8>b;th13test::emit(b,0,502,0,255,0,{0x20});th13test::emit(b,0,23,0,255,0,{1000});th13test::emit(b,0,10,0,255,0,{});bytes=th13test::build_scpt({{"main",b}},{"enemy.anm","st01enm.anm"});}
 bool read(const std::string&,std::vector<u8>&b)override{b=bytes;return true;}
 bool animation(u32,const std::string&n)override{names->push_back(n);return true;}
};

TEST(child_projectile_cannot_graze_before_its_first_manager_update) {
 for(bool delayed:{false,true}){
  Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
  p.names=&battle.sim.anim_list;CHECK(battle.load(p,"test.ecl",2,h,s,player,score));
  battle.sim.enemies.clear();battle.set_player_pos(0,400);s.max_point_cap=10000000;
  //Actual child birth during advance_bullets; parent itself has collision off.
  StageBullet parent;parent.x=30;parent.y=400;parent.flags=0;
  parent.ex[0].op=0x2000;parent.ex[0].integer_arguments[0]=i32(0x01000002u);
  parent.ex[0].integer_arguments[1]=1;parent.ex[1].op=0x4000;parent.ex[1].integer_arguments[0]=1;
  if(delayed){parent.ex[2].op=2;parent.ex[2].args[4]=1;}
  battle.sim.bullet_animation=[](StageBullet& b,bool){b.animation_ready=b.state!=2;return true;};
  battle.sim.bullets.push_back(parent);GameInput input;
  CHECK(battle.step(input));CHECK_EQ(battle.sim.bullets.size(),size_t(2));CHECK_EQ(s.graze,0);
  CHECK(!battle.sim.bullets[1].collision_ready);
  for(int i=0;i<(delayed?9:1);++i)CHECK(battle.step(input));
  CHECK_EQ(s.graze,1);CHECK(battle.sim.bullets[1].grazed);
 }
}
TEST(child_emissions_consume_random_in_original_newest_parent_order) {
 StageSim sim;sim.interactive=true;sim.random.reseed_restore(123);
 for(float x:{-100.f,100.f}){
  StageBullet parent;parent.x=x;parent.y=200;parent.flags=0;
  parent.ex[0].op=0x2000;parent.ex[0].integer_arguments[0]=i32(0x08000002u);
  parent.ex[0].integer_arguments[1]=1;parent.ex[0].args[0]=1;parent.ex[0].args[1]=2;
  parent.ex[1].op=0x4000;parent.ex[1].integer_arguments[0]=1;
  parent.ex[1].args[0]=-3;parent.ex[1].args[1]=3;
  sim.bullets.push_back(parent);
 }
 sim.advance_bullets();CHECK_EQ(sim.bullets.size(),size_t(4));
 //Original active list B->A: new child from B is emitted before child from A.
 CHECK_EQ(sim.bullets[2].x,100.f);CHECK_EQ(sim.bullets[3].x,-100.f);
 CHECK(!sim.bullets[2].collision_ready);CHECK(!sim.bullets[3].collision_ready);
 CHECK_EQ(sim.random.index(),8u);
}
TEST(deathbomb_suppresses_hit_and_cancellation_but_still_allows_graze) {
 for(bool circle:{false,true}) {
  Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
  p.names=&battle.sim.anim_list;CHECK(battle.load(p,"test.ecl",2,h,s,player,score));
  battle.sim.enemies.clear();battle.set_player_pos(0,200);
  battle.player_phase=StageBattle::PlayerPhase::Deathbomb;battle.player_phase_frame=1;
  for(float x:{0.f,20.f,80.f}){StageBullet b;b.x=x;b.y=200;b.flags=circle?0x12u:2u;b.hit_size[0]=b.hit_size[1]=4;b.collision_ready=true;b.alive=true;battle.sim.bullets.push_back(b);}
  GameInput input;CHECK(battle.step(input));
  CHECK_EQ(s.graze,1);CHECK(battle.sim.bullets[0].alive);CHECK(!battle.sim.bullets[0].grazed);
  CHECK(battle.sim.bullets[1].grazed);CHECK(!battle.sim.bullets[2].grazed);
  CHECK(battle.player_phase==StageBattle::PlayerPhase::Deathbomb);
 }
}
}
int main(){return th13test::run_all("child projectile collision phase");}
