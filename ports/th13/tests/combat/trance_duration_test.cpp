#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
using namespace th13;
namespace {
struct Host:StageBattleServices{
 std::vector<i32> sounds;std::vector<bool> music;
 bool life_hud(i32,i32)override{return true;}bool bomb_hud(i32,i32)override{return true;}
 bool request_game_over()override{return true;}
 bool hud_sound(i32 id)override{sounds.push_back(id);return true;}
 bool trance_music(bool active)override{music.push_back(active);return true;}
};
struct Provider:EclResourceProvider{
 std::vector<u8> bytes;std::vector<std::string>* names=nullptr;
 Provider(){std::vector<u8>b;th13test::emit(b,0,502,0,255,0,{0x20});th13test::emit(b,0,23,0,255,0,{1000});th13test::emit(b,0,10,0,255,0,{});bytes=th13test::build_scpt({{"main",b}},{"enemy.anm","st01enm.anm"});}
 bool read(const std::string&,std::vector<u8>&b)override{b=bytes;return true;}
 bool animation(u32,const std::string&n)override{names->push_back(n);return true;}
};

TEST(manual_trance_drains_one_point_per_frame_and_grants_exit_protection) {
 Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
 p.names=&battle.sim.anim_list;s.character=0;
 CHECK(battle.load(p,"test.ecl",2,h,s,player,score));battle.sim.enemies.clear();
 GameInput input;
 for(int i=0;i<30;++i)CHECK(battle.step(input));
 s.ending_points=600;input.update(0x800);CHECK(battle.step(input));
 CHECK(trance_active(s));CHECK_EQ(s.ending_points,600);
 CHECK_EQ(h.sounds.back(),0x34);CHECK_EQ(h.music.size(),size_t(1));CHECK(h.music.back());
 input.update(0);
 for(int i=0;i<599;++i)CHECK(battle.step(input));
 CHECK(trance_active(s));CHECK_EQ(s.ending_points,1);
 CHECK(battle.step(input));CHECK(!trance_active(s));CHECK_EQ(s.ending_points,0);
 //43d4f0 exits before443240 decrements player protection in this same update.
 CHECK_EQ(battle.invulnerable,69);
 CHECK(battle.step(input));CHECK_EQ(battle.invulnerable,68);
 CHECK_EQ(h.music.size(),size_t(2));CHECK(!h.music.back());
}
TEST(marisa_trance_scales_movement_on_activation_frame_in_fixed_units) {
 // 442960's character1/flag100 branch: Marisa replay f2321 y244.0859375
 // becomes236.5859375 on the same frame that the trance key is pressed.
 Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
 p.names=&battle.sim.anim_list;s.character=1;
 CHECK(battle.load(p,"test.ecl",2,h,s,player,score));battle.sim.enemies.clear();
 // This ECL-only fixture skips SHT loading; supply the verified pl01a.sht
 // fixed speeds (5 and3.535533905 before conversion to1/128 units).
 battle.speed_orth=640;battle.speed_diag=452;
 GameInput input;for(int i=0;i<30;++i)CHECK(battle.step(input));
 battle.set_player_pos(-61.0234375f,244.0859375f);
 s.ending_points=600;input.update(0xa10);CHECK(battle.step(input));
 CHECK(trance_active(s));CHECK_EQ(battle.player_x,-61.0234375f);
 CHECK_EQ(battle.player_y,236.5859375f);
 input.update(0x50);CHECK(battle.step(input));
 CHECK_EQ(battle.player_x,-66.3203125f);CHECK_EQ(battle.player_y,231.2890625f);
 // 0x442a19-0x442a57 focus 三重门：stage ctl 非空、ctl+0xb8（存活敌机
 // 链计数）非 0、player+0x67c>=4 同时成立才保留 Shift 位；否则 focus=0
 // 并写 +0x14680=0x1e。本 fixture 在 enemies.clear() 后链计数为 0，
 // 原生同门关闭——0x98（Shift+上+右）走非聚焦对角 452×3/2=678（1/128）
 // =5.296875 px/帧，而不是聚焦 181×3/2=271=2.1171875。
 input.update(0x98);CHECK(battle.step(input));
 CHECK_EQ(battle.player_x,-61.0234375f);CHECK_EQ(battle.player_y,225.9921875f);
 CHECK(battle.end_trance());input.update(0x10);CHECK(battle.step(input));
 CHECK_EQ(battle.player_y,220.9921875f);
}
TEST(deathbomb_window_trance_save_survives_as_manual_trance) {
 // 0x4057f0 主动激活门无 player state 检查：决死窗（state4）内满计按 C
 // 照常激活手动灵界。窗口到期 0x443240 case4 → 0x444a10 → 0x405a20(1)
 // 走 already-active 早退（0x405b64 xor eax,eax）返回 0 → 存活（0x65c=1，
 // 不扣命、timer 保留），bit1 不动 → 结束按手动语义 -200，不再死亡。
 Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
 p.names=&battle.sim.anim_list;s.character=0;
 CHECK(battle.load(p,"test.ecl",2,h,s,player,score));battle.sim.enemies.clear();
 GameInput input;for(int i=0;i<30;++i)CHECK(battle.step(input));
 s.ending_points=600;const auto lives=player.extra_lives;
 battle.player_phase=StageBattle::PlayerPhase::Deathbomb;battle.player_phase_frame=0;
 input.update(0x800);CHECK(battle.step(input)); // C 在决死窗首帧
 CHECK(trance_active(s));CHECK_EQ(h.sounds.back(),0x34);CHECK(h.music.back());
 CHECK(battle.player_phase==StageBattle::PlayerPhase::Deathbomb); // 窗口未过期
 for(int i=0;i<8;++i){input.update(0);CHECK(battle.step(input));} // timer 0..8
 // 到期帧 commit_death：already-active 早退 → 存活，不扣命，灵界不被吞。
 CHECK(battle.player_phase==StageBattle::PlayerPhase::Active);
 CHECK_EQ(player.extra_lives,lives);CHECK_EQ(player.deaths,0);
 CHECK(trance_active(s));CHECK(s.ending_points>0);
 for(int i=0;i<600;++i)CHECK(battle.step(input)); // 手动语义耗完 600 帧
 CHECK(!trance_active(s));CHECK_EQ(s.ending_points,0);
 CHECK(battle.player_phase==StageBattle::PlayerPhase::Active);
 CHECK_EQ(player.extra_lives,lives);CHECK_EQ(player.deaths,0);
 CHECK_EQ(h.music.size(),size_t(2));CHECK(!h.music.back());
}
TEST(deathbomb_window_trance_save_keeps_gauge_if_window_outlives_activation) {
 // 到期帧同帧按 C：0x4057f0（激活）先于 0x443240 case4（到期判定），
 // 与原版 42c550→4057f0→443240 同序——最迟一帧的决死灵界同样保命。
 Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
 p.names=&battle.sim.anim_list;s.character=0;
 CHECK(battle.load(p,"test.ecl",2,h,s,player,score));battle.sim.enemies.clear();
 GameInput input;for(int i=0;i<30;++i)CHECK(battle.step(input));
 s.ending_points=600;const auto lives=player.extra_lives;
 battle.player_phase=StageBattle::PlayerPhase::Deathbomb;battle.player_phase_frame=8;
 input.update(0x800);CHECK(battle.step(input)); // 到期帧同帧按 C
 CHECK(trance_active(s));
 CHECK(battle.player_phase==StageBattle::PlayerPhase::Active);
 CHECK_EQ(player.extra_lives,lives);CHECK_EQ(player.deaths,0);
}
TEST(protective_trance_expiry_commits_death_without_a_second_deathbomb_window) {
 Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
 p.names=&battle.sim.anim_list;s.character=0;
 CHECK(battle.load(p,"test.ecl",2,h,s,player,score));battle.sim.enemies.clear();
 s.ending_points=200;const auto lives=player.extra_lives;
 battle.player_phase=StageBattle::PlayerPhase::Deathbomb;battle.player_phase_frame=8;
 GameInput input;CHECK(battle.step(input));CHECK(trance_active(s));
 CHECK(battle.player_phase==StageBattle::PlayerPhase::Active);CHECK_EQ(player.extra_lives,lives);
 CHECK_EQ(s.ending_points,200); //activation inside443240 follows this frame's drain
 for(int frame=0;frame<200;++frame)CHECK(battle.step(input));
 CHECK(!trance_active(s));CHECK(battle.player_phase==StageBattle::PlayerPhase::Dying);
 CHECK_EQ(player.extra_lives,lives-1);CHECK_EQ(player.deaths,1);CHECK_EQ(battle.invulnerable,69);
 const auto bombs=player.bombs;input.update(2);CHECK(battle.step(input));
 CHECK(battle.player_phase==StageBattle::PlayerPhase::Dying);CHECK_EQ(player.bombs,bombs);
}
TEST(spell_practice_does_not_enter_protective_trance_on_death) {
 Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
 p.names=&battle.sim.anim_list;s.character=0;
 CHECK(battle.load(p,"test.ecl",2,h,s,player,score));battle.sim.enemies.clear();
 s.run_mode_flags=0x20;s.ending_points=600;const auto lives=player.extra_lives;
 battle.player_phase=StageBattle::PlayerPhase::Deathbomb;battle.player_phase_frame=8;
 GameInput input;CHECK(battle.step(input));CHECK(!trance_active(s));
 CHECK(battle.player_phase==StageBattle::PlayerPhase::Dying);CHECK_EQ(player.extra_lives,lives-1);
}
TEST(final_stage_final_chapter_keeps_base_music_during_trance) {
 Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
 p.names=&battle.sim.anim_list;s.character=0;
 CHECK(battle.load(p,"test.ecl",2,h,s,player,score));battle.sim.enemies.clear();
 GameInput input;for(int n=0;n<30;++n)CHECK(battle.step(input));
 s.stage=6;s.chapter=6;s.ending_points=600;input.update(0x800);CHECK(battle.step(input));
 CHECK(trance_active(s));CHECK_EQ(h.sounds.back(),0x34);CHECK(h.music.empty());
 CHECK(battle.end_trance());CHECK(h.music.empty());
}
TEST(protective_trance_entry_keeps_position_and_executes_the_shared_death_spirit_branch) {
 Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
 p.names=&battle.sim.anim_list;s.character=0;
 CHECK(battle.load(p,"test.ecl",2,h,s,player,score));battle.sim.enemies.clear();
 battle.set_player_pos(-84.46875f,418.5859375f);s.ending_points=550;
 battle.player_phase=StageBattle::PlayerPhase::Deathbomb;battle.player_phase_frame=8;
 const auto calls=battle.sim.random.index();GameInput input;input.update(0x98);
 CHECK(battle.step(input));CHECK(trance_active(s));
 CHECK_EQ(battle.player_x,-84.46875f);CHECK_EQ(battle.player_y,418.5859375f);
 CHECK_EQ(battle.player_phase_frame,9);CHECK_EQ(battle.sim.random.index(),calls+4);
 const auto& spirit=battle.item_manager.spirits()[0];
 CHECK(spirit.alive);CHECK_EQ(spirit.kind,3);CHECK_EQ(spirit.flags&1u,1u);
 CHECK_EQ(s.ending_points,550);
 // 0x442a19-0x442a57：enemies.clear() 后 focus 门关闭（ctl+0xb8==0），
 // Shift+右走非聚焦正交 576（1/128）=4.5 px/帧；灵梦无 3/2 缩放。
 input.update(0x88);CHECK(battle.step(input));CHECK_EQ(battle.player_x,-79.96875f);
 CHECK_EQ(battle.player_y,418.5859375f);CHECK_EQ(s.ending_points,549);
}
TEST(respawn_damage_pulse_matches_native_growth_lifetime_and_hits) {
 Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
 p.names=&battle.sim.anim_list;CHECK(battle.load(p,"test.ecl",2,h,s,player,score));
 battle.sim.enemies.clear();battle.set_player_pos(-40.546875f,220.0546875f);
 battle.player_phase=StageBattle::PlayerPhase::Dying;battle.player_phase_frame=30;
 player.extra_lives=2;score.score=0;GameInput input;
 StageEnemy target;target.position[0]=-31.6f;target.position[1]=154;
 target.life=100000;target.hurtbox[0]=target.hurtbox[1]=24;target.spirit_threshold=1000000;
 for(int frame=0;frame<31;++frame){
  CHECK(battle.step(input));
  CHECK_EQ(battle.respawn_damage.active,frame<29);
  CHECK_EQ(battle.respawn_damage.center.x,-40.55f);
  CHECK_EQ(battle.respawn_damage.center.y,220.05f);
  CHECK_EQ(battle.respawn_damage.radius,float(32+16*std::min(frame+1,30)));
  const int remaining=std::max(29-frame,0),expected_score=std::min(std::max(frame-1,0),27);
  CHECK_EQ(battle.respawn_damage.remaining.current,remaining);
  const float before=target.life;battle.sim.enemy_hit(target);
  CHECK_EQ(before-target.life,frame>=2&&frame<29?16.f:0.f); // entering damage /5 after capped80
  CHECK_EQ(score.score,expected_score);
  if(frame==0){CHECK_EQ(battle.invulnerable,279);CHECK_EQ(battle.player_y,480.f);}
 }
}
TEST(death_frame_three_drops_seven_power_items_with_native_angles) {
 Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
 p.names=&battle.sim.anim_list;CHECK(battle.load(p,"test.ecl",2,h,s,player,score));
 battle.sim.enemies.clear();battle.set_player_pos(-40.546875f,220.0546875f);
 battle.player_phase=StageBattle::PlayerPhase::Dying;battle.player_phase_frame=3;player.power=400;
 GameInput input;CHECK(battle.step(input));CHECK_EQ(player.power,300);
 const u32 angle_words[]={3219417070u,3218475870u,3217534670u,3216593471u,3215652271u,3214711072u,3213769872u};
 for(int i=0;i<7;++i){
  float angle;std::memcpy(&angle,&angle_words[i],4);
  const auto& item=battle.item_manager.items()[599-i];
  CHECK(item.alive);CHECK_EQ(item.type,1);
  CHECK_EQ(item.vx,float(std::cos(double(angle))*3.));
  CHECK_EQ(item.vy,float(float(std::sin(double(angle))*3.)+.03f));
 }
}
TEST(respawn_entry_clears_bullets_only_after_thirty_updates_without_cancel_items) {
 Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle battle;
 p.names=&battle.sim.anim_list;CHECK(battle.load(p,"test.ecl",2,h,s,player,score));battle.sim.enemies.clear();
 battle.player_phase=StageBattle::PlayerPhase::Entering;battle.player_phase_frame=29;
 battle.respawn_origin={100,100,0};
 StageBullet bullet;bullet.x=100;bullet.y=200;bullet.vel[0]=2;bullet.sprite=0;bullet.subtype=4;
 battle.sim.bullets.push_back(bullet);int effects=0;
 battle.sim.stationary_cancel_effect=[&](i32 script,const Vec3& pos){++effects;CHECK_EQ(script,139);CHECK_EQ(pos.x,102.f);return true;};
 GameInput input;CHECK(battle.step(input));CHECK_EQ(battle.sim.bullets[0].state,1);CHECK_EQ(effects,0);
 CHECK(battle.step(input));CHECK_EQ(battle.sim.bullets[0].state,4);CHECK_EQ(effects,1);
 CHECK_EQ(battle.sim.bullets[0].x,103.f);
 // 契约更正（0x40db10 尾部门控，主会话裁决 2026-11-01）：碰撞的实际门是
 // 每帧 collide_now（仅 state==1 的弹在 case1 尾按 +0x20 bit1 调
 // 0x444140/0x443fd0）；collision_ready 是 state1/2 的簿记字段，state4
 // 消费弹不参与任何帧的碰撞调用。废弹不碰撞的可观察证据 = 下述位移冻结
 // （x 恒 103，state4 不再步进位移）。
 for(int i=0;i<3;++i)CHECK(battle.step(input));
 CHECK_EQ(battle.sim.bullets[0].x,103.f);CHECK_EQ(battle.sim.bullets[0].state,4);CHECK_EQ(effects,1);
 for(const auto& item:battle.item_manager.items())CHECK(!item.alive);
}
}
int main(){return th13test::run_all("trance duration");}
