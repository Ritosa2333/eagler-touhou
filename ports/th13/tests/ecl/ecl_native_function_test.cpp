// H-NATIVE-FUNCTION: 41d137/41d15d/41d1bb ->41a8a0 ->4bb9fc table.
#include "ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
using namespace th13;
namespace {
struct Provider:EclResourceProvider {
 std::vector<u8> data;
 std::vector<std::string>* names=nullptr;
 Provider(){std::vector<u8>code;th13test::emit(code,0,502,0,255,0,{0x20});th13test::emit(code,0,23,0,255,0,{1000});data=th13test::build_scpt({{"main",code}},{"enemy.anm","st01enm.anm"});}
 bool read(const std::string&,std::vector<u8>&out)override{out=data;return true;}
 bool animation(u32,const std::string& name)override{names->push_back(name);return true;}
};
struct Host:StageBattleServices {
 std::vector<int> sounds;
 bool life_hud(i32,i32)override{return true;}bool bomb_hud(i32,i32)override{return true;}
 bool request_game_over()override{return true;}bool hud_sound(i32 s)override{sounds.push_back(s);return true;}
};
int command(StageSim& sim,StageEnemy& e,u16 op,i32 id){EclContext c;std::vector<u8>b;th13test::emit(b,0,op,0,255,0,{u32(id)});c.instruction=reinterpret_cast<const EclInstruction*>(b.data());sim.current=&e;return sim.commands.execute(c,op);}
TEST(callback_registration_clear_and_immediate_invocation) {
 StageSim sim;sim.commands.sim=&sim;StageEnemy e;StageBullet b;b.x=10;b.y=100;b.ex_cursor=2;e.f_slots[0]=48;sim.player_x=0;sim.player_y=100;sim.bullets.push_back(b);
 CHECK_EQ(command(sim,e,632,5),0);CHECK_EQ(e.native_function,5);CHECK_EQ(e.native_function_mode,0);
 CHECK_EQ(sim.bullets[0].ex_cursor,2);CHECK_EQ(command(sim,e,637,5),0);CHECK_EQ(sim.bullets[0].ex_cursor,5);
 sim.bullets[0].ex_cursor=9;CHECK_EQ(command(sim,e,637,5),0);CHECK_EQ(sim.bullets[0].ex_cursor,9);
 CHECK_EQ(command(sim,e,639,1),0);CHECK_EQ(e.native_function_mode,1);
 CHECK_EQ(command(sim,e,632,0),0);CHECK_EQ(e.native_function,0);
}
TEST(rectangle_and_moving_wall_reflect_bullets_without_ex_bounce_counter) {
 StageSim sim;StageEnemy e;e.f_slots[0]=-64;e.f_slots[1]=64;e.f_slots[2]=40;e.f_slots[3]=180;
 StageBullet b;b.x=65;b.y=100;b.vel[0]=4;b.speed=4;b.angle=0;b.transform_sound=38;sim.bullets.push_back(b);int sounds=0;sim.enemy_sound=[&](i32 s){CHECK_EQ(s,38);++sounds;return true;};
 CHECK(sim.invoke_enemy_function(e,1));CHECK_EQ(sim.bullets[0].x,63.f);CHECK(sim.bullets[0].vel[0]<0);CHECK_EQ(sounds,1);CHECK_EQ(sim.bullets[0].ex40_bounces,0);
 e.f_slots[0]=-32;e.f_slots[1]=32;sim.bullets[0]=b;sim.bullets[0].x=-31;
 CHECK(sim.invoke_enemy_function(e,2));CHECK_EQ(sim.bullets[0].x,-33.f);
}
TEST(stage3_absorption_heals_spell_accumulator_and_excludes_movement_states) {
 Provider provider;Host host;SessionState session;HudPlayerState player;HudScoreState score;StageBattle battle;
 provider.names=&battle.sim.anim_list;
 CHECK(battle.load(provider,"fixture.ecl",2,host,session,player,score));StageEnemy e;e.life=100;e.spell_active=true;e.spell_life=700;e.i_slots[2]=150;e.position[1]=100;
 auto& spirits=battle.item_manager.spirits();
 for(int i=0;i<4;++i){auto&s=spirits[i];s.alive=true;s.state=i==2?2:1;s.kind=i==3?3:1;s.x=i==0?0:30;s.y=100;}
 CHECK(battle.sim.invoke_enemy_function(e,4));CHECK_EQ(e.spell_life,850);CHECK_EQ(e.life,121.f);CHECK_EQ(e.i_slots[3],1);
 CHECK(!spirits[0].alive);CHECK_EQ(spirits[1].state,5);CHECK_EQ(spirits[2].state,2);CHECK_EQ(spirits[3].state,1);
 CHECK_EQ(host.sounds.size(),2u);CHECK_EQ(host.sounds[0],63);CHECK_EQ(host.sounds[1],67);
}
TEST(extra_callback_spawns_real_enemy_and_consumes_trigger_once) {
 StageSim sim;CHECK(sim.load("artifacts/reference/raw","artifacts/verification/enemy-functions-20261027/extra-events.jsonl","st07.ecl"));StageEnemy e;e.i_slots[0]=19;sim.player_y=200;
 StageBullet b;b.x=32;b.y=203.99f;b.generation=500;sim.bullets.push_back(b);b.y=204;b.generation=501;sim.bullets.push_back(b);
 const auto count=sim.enemies.size();CHECK(sim.invoke_enemy_function(e,6));CHECK(sim.error().empty());CHECK_EQ(sim.enemies.size(),count+1);
 CHECK_EQ(sim.bullets[0].native_trigger,1);CHECK_EQ(sim.bullets[0].state,3);CHECK_EQ(sim.bullets[1].native_trigger,0);
 CHECK_EQ(sim.enemies.back()->routine,std::string("BossCard2_EtAt"));CHECK_EQ(sim.enemies.back()->i_slots[0],19);
 CHECK(sim.invoke_enemy_function(e,6));CHECK_EQ(sim.enemies.size(),count+1);
}
TEST(registered_callback_runs_after_constructor_ecl_before_damage) {
 Provider provider;Host host;SessionState session;HudPlayerState player;HudScoreState score;StageBattle battle;
 std::vector<u8> main,trigger;th13test::emit(main,0,23,0,255,0,{1000});
 th13test::emit(trigger,0,632,0,255,0,{5});th13test::emit(trigger,0,23,0,255,0,{1000});
 th13test::emit(main,1000,10,0,255,0,{});th13test::emit(trigger,1000,10,0,255,0,{});
 provider.data=th13test::build_scpt({{"main",main},{"trigger",trigger}},{"enemy.anm","st01enm.anm"});
 provider.names=&battle.sim.anim_list;CHECK(battle.load(provider,"fixture.ecl",2,host,session,player,score));
 auto& sim=battle.sim;sim.player_x=0;sim.player_y=100;StageBullet b;b.x=0;b.y=100;b.ex_cursor=2;sim.bullets.push_back(b);
 bool damage_saw_callback=false;sim.enemy_hit=[&](StageEnemy&){damage_saw_callback=sim.bullets[0].ex_cursor==5;};
 StageSpawnArguments args;args.floats[0]=48;
 auto* e=sim.spawn_enemy("trigger",0,100,100,nullptr,false,false,&args);
 CHECK(e);CHECK(damage_saw_callback);CHECK_EQ(sim.bullets[0].native_trigger,1);
 CHECK_EQ(command(sim,*e,632,0),0);sim.bullets[0].native_trigger=0;sim.bullets[0].ex_cursor=2;
 for(int i=0;i<3;++i)CHECK(sim.step_frame());CHECK_EQ(sim.bullets[0].ex_cursor,2);
}
TEST(absorbed_spirit_homes_to_boss_then_returns_to_player_within_30px) {
 ItemManager items;SessionState session;HudPlayerState player;HudScoreState score;
 ItemCollector c;c.progress=&session;c.player=&player;c.score=&score;c.x=-100;c.y=300;
 c.boss_present=true;c.boss_x=100;c.boss_y=100;
 auto& s=items.spirits()[0];s.alive=true;s.state=5;s.kind=1;s.x=0;s.y=100;s.speed=2;
 items.update_spirits(c);CHECK_EQ(s.angle,0.f);CHECK_EQ(s.move_speed,2.f);CHECK_EQ(s.x,2.f);
 CHECK(s.speed>2);CHECK_EQ(s.state,5);
 c.x=s.x+29;c.y=s.y;items.update_spirits(c);CHECK_EQ(s.state,2);
 s.state=5;c.x=-100;c.y=300;c.boss_present=false;items.update_spirits(c);CHECK_EQ(s.state,1);
}
TEST(real_stage2_boss_attack_registers_and_applies_moving_wall_callback) {
 StageSim sim;CHECK(sim.load("artifacts/reference/raw","artifacts/verification/enemy-functions-20261027/stage2-events.jsonl","st02.ecl"));
 StageBullet bullet;bullet.x=193;bullet.y=100;bullet.speed=4;bullet.vel[0]=4;bullet.angle=0;bullet.transform_sound=-1;
 sim.bullets.push_back(bullet);
 auto* enemy=sim.spawn_enemy("Boss2_at",0,128,1000,nullptr,false);
 CHECK(sim.error().empty());CHECK(enemy);if(!enemy)return;
 CHECK_EQ(enemy->native_function,2);CHECK_EQ(sim.bullets[0].x,191.f);CHECK(sim.bullets[0].vel[0]<0);
 const u32 serial=enemy->serial;
 for(sim.frame=1;sim.frame<=300;++sim.frame)CHECK(sim.step_frame());
 CHECK(sim.error().empty());
 // The isolated subroutine returns and its owner is removed, unlike the
 // real synchronous Boss2 caller. Do not dereference that retired owner.
 for(const auto& live:sim.enemies)CHECK(live->serial!=serial);
}
}
int main(){return th13test::run_all("native-functions");}
