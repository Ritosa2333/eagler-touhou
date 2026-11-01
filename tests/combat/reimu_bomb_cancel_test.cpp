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
TEST(reimu_orbit_clear_spawns_real_cancel_item_before_bullet_motion) {
 Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;p.names=&battle.sim.anim_list;s.character=0;
 CHECK(battle.load(p,"test.ecl",2,h,s,player,score));
 StageBullet b;b.x=0;b.y=400;b.vel[0]=2;b.hit_size[0]=4;b.generation=1;battle.sim.bullets.push_back(b);
 GameInput in;in.update(2);CHECK(battle.step(in));
 CHECK_EQ(battle.sim.bullets.size(),size_t(1));CHECK(battle.sim.bullets[0].alive);
 CHECK_EQ(battle.sim.bullets[0].state,4);CHECK_EQ(battle.sim.bullets[0].x,1.f);
 size_t found=0;for(const auto& item:battle.item_manager.items())if(item.alive){++found;CHECK_EQ(item.type,9);CHECK_EQ(item.x,0.f);CHECK_EQ(item.y,400.f);}
 CHECK_EQ(found,1u);
}
TEST(reimu_orb_births_precede_cancellation_and_preserve_slot_order) {
 Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
 p.names=&battle.sim.anim_list;s.character=0;CHECK(battle.load(p,"test.ecl",2,h,s,player,score));
 StageBullet b;b.x=0;b.y=400;b.generation=1;battle.sim.bullets.push_back(b);
 int births=0;
 battle.reimu_bomb_orb_birth=[&](StageBattle& active,i32 index){
  CHECK_EQ(index,births);CHECK_EQ(active.sim.bullets[0].state,1);
  CHECK_EQ(active.bomb_orbs[size_t(index)].x,0.f);
  CHECK_EQ(active.bomb_orbs[size_t(index)].y,400.f);++births;return true;
 };
 GameInput input;input.update(2);CHECK(battle.step(input));CHECK_EQ(births,8);
 CHECK_EQ(battle.sim.bullets[0].state,4);
}
TEST(reimu_damage_explosions_and_end_register_separate_shakes) {
 Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
 p.names=&battle.sim.anim_list;s.character=0;CHECK(battle.load(p,"test.ecl",2,h,s,player,score));
 int shakes=0;battle.reimu_explosion_shake=[&](){++shakes;return true;};
 GameInput input;input.update(2);CHECK(battle.step(input));CHECK_EQ(shakes,0);
 for(int i=0;i<2;++i) {
  const auto slot=battle.bomb_orbs[i].registration_slot;
  CHECK(slot>=0);battle.damage_registrations[size_t(slot)].accumulated_damage=300;
 }
 input.update(0);CHECK(battle.step(input));CHECK_EQ(shakes,2);
 CHECK(!battle.bomb_orbs[0].active);CHECK(!battle.bomb_orbs[1].active);
 battle.bomb_age=199;battle.bomb_timer=1;
 CHECK(battle.step(input));CHECK_EQ(shakes,3); // one for all remaining orbs
 CHECK(battle.step(input));CHECK_EQ(shakes,3);
}
TEST(reimu_cancel_radius_includes_bullet_half_size_and_skips_canceled_nodes) {
 StageSim sim;ItemManager items;sim.items=&items;StageBullet b;b.x=66;b.y=128;b.hit_size[0]=4;b.vel[0]=2;sim.bullets.push_back(b);b.x=66.01f;sim.bullets.push_back(b);b.x=0;b.state=3;sim.bullets.push_back(b);
 sim.cancel_bullets_radius(Vec3{0,128,0},64,true);
 CHECK(sim.bullets[0].alive);CHECK_EQ(sim.bullets[0].state,4);CHECK_EQ(sim.bullets[0].x,67.f);
 CHECK(sim.bullets[1].alive);CHECK_EQ(sim.bullets[1].state,1);CHECK_EQ(sim.bullets[1].x,66.01f);
 CHECK(sim.bullets[2].alive);CHECK_EQ(sim.bullets[2].state,3);
 size_t found=0;for(const auto&i:items.items())if(i.alive){++found;CHECK_EQ(i.type,9);CHECK_EQ(i.x,66.f);}
 CHECK_EQ(found,1u);
}
TEST(deathbomb_restores_active_state_without_moving_until_next_frame) {
 for(bool deathbomb:{false,true}) {
  Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
  p.names=&battle.sim.anim_list;s.character=0;
  CHECK(battle.load(p,"test.ecl",2,h,s,player,score));
  battle.restore_replay_player(-2065,54223,false);
  battle.player_phase=deathbomb?StageBattle::PlayerPhase::Deathbomb:StageBattle::PlayerPhase::Active;
  battle.player_phase_frame=3;battle.invulnerable=deathbomb?4:0;
  GameInput input;input.update(0x12); // up+Bomb, Stage5 replay input5935
  CHECK(battle.step(input));CHECK(battle.player_phase==StageBattle::PlayerPhase::Active);
  CHECK_EQ(battle.player_y,deathbomb?423.6171875f:419.1171875f);
  CHECK(battle.bomb_age>=0);
  input.update(0x12);CHECK(battle.step(input));
  CHECK_EQ(battle.player_y,deathbomb?419.1171875f:414.6171875f);
 }
}
}
int main(){return th13test::run_all("reimu-bomb-cancel");}
