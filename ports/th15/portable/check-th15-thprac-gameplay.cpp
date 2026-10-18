// Typed game-owner regression; fake platform services deliberately exclude
// GPU/audio/input/device equivalence. Native instruction sites are checked
// separately in check-th15-thprac-native.mjs.
#include "../th15_web/cpp/game/SpellCard.hpp"
#include "../th15_web/cpp/game/ChapterCheckpoint.hpp"
#include "../th15_web/cpp/game/SessionRuntime.hpp"
#include "../th15_web/cpp/game/PracticeAbScores.hpp"
#include "../th15_web/cpp/game/SpellDraw.hpp"
#include <cassert>
#include <iostream>
using namespace th15;
namespace th15::Localization {const char* SpellName(u32,const char* fallback,u32){return fallback;}}
struct SpellHost final:SpellCardHost {
 u32 next=1;
 bool background_visible(bool)override{return true;}bool prepare_hud(bool)override{return true;}
 u32 create_visual(i32,i32)override{return next++;}bool interrupt(u32,i32)override{return true;}
 bool retire(u32& h)override{h=0;return true;}bool text(u32,const std::string&)override{return true;}
 bool child_integer(u32,i32,i32,i32)override{return true;}bool position(u32,const Vec3&)override{return true;}
 bool boss_position(Vec3& p)override{p={0,100,0};return true;}bool history_begin(i32,const std::string&)override{return true;}
 bool history_capture(i32)override{return true;}bool result(i32,bool)override{return true;}bool sound(i32)override{return true;}
};
struct CheckpointHost final:ChapterCheckpointServices {
 EnemyWorldState& world;int captured_total=-1;bool sync=true;
 explicit CheckpointHost(EnemyWorldState& w):world(w){}
 bool synchronize_resources()override{return sync;}bool clear_saved_animations()override{return true;}
 bool save_player()override{return true;}bool save_enemies()override{captured_total=world.chapter_total;return true;}
 bool save_background()override{return true;}bool save_bullets()override{return true;}bool save_items()override{return true;}
 bool save_effects()override{return true;}bool save_popups()override{return true;}bool save_bomb()override{return true;}
 bool retire_reward()override{return true;}bool retire_message()override{return true;}bool retire_scene_effect()override{return true;}
 bool restore_player()override{return true;}bool restore_enemies()override{return true;}bool restore_background()override{return true;}
 bool restore_bullets()override{return true;}bool restore_items()override{return true;}bool reset_spell()override{return true;}
 bool clear_lasers()override{return true;}bool restore_effects()override{return true;}bool restore_popups()override{return true;}bool restore_bomb()override{return true;}
 bool life_hud(i32,i32)override{return true;}bool bomb_hud(i32,i32)override{return true;}bool reset_gui()override{return true;}
 bool stop_sounds()override{return true;}bool begin_restart_effect()override{return true;}bool begin_restart_overlay()override{return true;}
 bool checkpoint_file(bool)override{return true;}
};
struct SessionHost final:SessionRuntimeServices {
 PracticePatchEffects effects;int rewards=0,snapshots=0,last_chapter=-1;bool last_boss=false;
 bool ending_fade()override{return true;}bool finish_replay()override{return true;}bool destination(SessionDestination)override{return true;}
 bool load_scene(bool& waiting)override{waiting=false;return true;}bool activate_scene()override{return true;}bool release_background()override{return true;}
 bool load_checkpoint_file(bool& restored)override{restored=false;return true;}bool restart_overlay(i32)override{return true;}
 bool restart_effect(i32)override{return true;}bool prepare_stage_music()override{return true;}bool start_stage_music()override{return true;}
 bool start_boss_music()override{return true;}bool seek_stage_music(double)override{return true;}bool demo_fade()override{return true;}
 bool update_score()override{return true;}bool chapter_reward(bool boss)override{++rewards;last_boss=boss;return true;}
 bool chapter_checkpoint(i32 c)override{++snapshots;last_chapter=c;return true;}
 bool skip_chapter_reward()override{return effects.skip_reward();}bool update_overlays()override{return true;}
};
int main(){
 // Exact source range formula, default and extrema. Preserve the lower
 // screen edge, and never shrink the range repeatedly each update frame.
 for(float amount:{0.f,.25f,.5f,1.f}){float center=100,range=80;practice_boss_range(&center,&range,amount);assert(center+range*.5f==140&&range==80*(1-amount));}
 assert(practice_boss_range_default==.5f);
 PracticePatchEffects sync;sync.stars_bgm_sync=true;
 assert(sync.music_start_offset(false)==0&&sync.stars_bgm_sync);
 assert(sync.music_start_offset(true)==practice_stars_bgm_byte_offset&&!sync.stars_bgm_sync);
 assert(sync.music_start_offset(true)==0);
 PracticeState counter;counter.enabled=true;EnemyWorldState counter_world;counter_world.practice=&counter;
 EnemyState boss;boss.flags=0x800000;boss.life=100;boss.interrupts[0]={10,600,"next","timeout"};
 assert(!boss.check_interrupt(counter_world)&&counter.lock_timer.pending);
 assert(!boss.check_interrupt(counter_world));counter.lock_timer.draw_tick();assert(counter.lock_timer.frames==1&&!counter.lock_timer.pending);
 for(int n=0;n<4;n++)counter.lock_timer.draw_tick();assert(counter.lock_timer.frames==1); // Extra high-refresh draws.
 counter.lock_timer.reset();assert(counter.lock_timer.frames==0);
 boss.flags=0;assert(!boss.check_interrupt(counter_world)&&!counter.lock_timer.pending);
 boss.flags=0x800000;counter.enabled=false;assert(!boss.check_interrupt(counter_world)&&!counter.lock_timer.pending);
 counter.enabled=true;boss.interrupts[0].time=0;assert(!boss.check_interrupt(counter_world)&&!counter.lock_timer.pending);
 // Source reset only clears the count, not an observation already made.
 counter.lock_timer.observe();counter.lock_timer.reset();assert(counter.lock_timer.pending);counter.lock_timer.draw_tick();assert(counter.lock_timer.frames==1);
 PracticeMusic bgm;
 assert(!bgm.suppress(true,true,PracticeMusic::play_addr,1));
 assert(bgm.mElStatus&&bgm.mLockBgmId==1);
 assert(bgm.suppress(true,true,0,0)); // Other includes preload/fade/reset.
 assert(bgm.suppress(true,true,PracticeMusic::play_addr,1));
 assert(bgm.suppress(true,true,PracticeMusic::pause_addr,0));
 assert(bgm.suppress(true,true,PracticeMusic::resume_addr,0));
 assert(!bgm.suppress(true,true,PracticeMusic::play_addr,0)); // Different id releases lock.
 assert(!bgm.mElStatus&&bgm.mLockBgmId==-1);
 assert(!bgm.suppress(true,true,PracticeMusic::play_addr,0));
 assert(!bgm.suppress(false,true,PracticeMusic::pause_addr,0)); // F7 off while paused.
 assert(!bgm.suppress(true,true,PracticeMusic::resume_addr,0)); // Re-arm allows one resume.
 assert(bgm.suppress(true,true,PracticeMusic::stop_addr,0)&&bgm.mLockBgmId==-1);
 // Stop clears the remembered id but retains mElStatus in native bit-1
 // mode. The next Play re-locks and remains suppressed, even for a new id.
 assert(bgm.suppress(true,true,PracticeMusic::play_addr,1)&&bgm.mLockBgmId==1);
 assert(!bgm.suppress(true,false,PracticeMusic::stop_addr,0)); // bit 1 false on normal exit.
 assert(!bgm.mElStatus&&bgm.mLockBgmId==-1);
 assert(!bgm.suppress(false,true,PracticeMusic::play_addr,0));
 assert(!bgm.suppress(false,true,PracticeMusic::stop_addr,0));
 const auto zero=practice_ab_scores({3,3,3,3,3}),unit=practice_ab_scores({6.5f,7.8f,7,6.1f,6});
 for(int i=0;i<6;i++){assert(std::abs(zero[i]-1.f/(1.f+std::exp(1.8f)))<1e-6f);assert(std::abs(unit[i]-1.f/(1.f+std::exp(-2.2f)))<1e-6f);}
 // Reaction phase has exactly zero contribution to movement and return
 // scores in purple. Do not replace its weights with an averaged rating.
 const auto reaction=practice_ab_scores({6.5f,3,3,3,3});assert(reaction[0]>zero[0]&&reaction[1]==zero[1]&&reaction[2]==zero[2]);
 PracticeState practice;practice.enabled=true;practice.cheats=PracticeLives|PracticeTime;
 assert(practice.preserve_life(0)&&!practice.preserve_life(8)&&!practice.preserve_life(-1));
 practice.map_inf_life_to_no_continue=false;assert(practice.preserve_life(8));
 practice.replay=true;assert(!practice.preserve_life(0)&&!practice.cheat(PracticeTime));practice.replay=false;
 SpellHost sh;PlayerSpellStatus status;int score=0;SpellCard spell(status,score,sh);
 SpellStartContext start;start.stage=1;start.difficulty=2;SpellStartRequest request;request.duration=1800;request.name="test";
 assert(spell.begin(request,start));spell.age.set(300);const int before=status.bonus,frames=spell.active_frames;
 assert(spell.update({1,200,0,true}));assert(status.bonus==before&&spell.age.current==301&&status.frame==301&&spell.active_frames==frames+1);
 assert(spell.update({1,200,0,false}));assert(status.bonus<before&&spell.age.current==302);
 // Test the production SpellDraw caption owner at both native thresholds.
 // GPU text coordinates and layout remain separate live-test obligations.
 for(auto pair:{std::pair<int,int>{2,3},{99,100},{100,123},{999,1234}}){
  for(bool complete:{false,true}){
   const auto digits=[](int n){return n<10?"0"+std::to_string(n):std::to_string(n);};
   const auto expected=!complete&&pair.first>=100?"MASTER":digits(pair.first)+"/"+(!complete&&pair.second>=100?"99+":digits(pair.second));
   assert(spell_history_caption(pair.first,pair.second,complete)==expected);
  }
 }
 // Skip reward even on chapter zero, then capture every nonnegative chapter.
 SessionState progress;PlayerLifeSession player;SessionHost host;SessionRuntime runtime(progress,player,host);runtime.age.set(31);host.effects.chapter_disable=2;
 for(int chapter:{0,43,44}){progress.scene_flags|=0x8000;runtime.requested_chapter=chapter;assert(runtime.update({})==i32(FrameAction::Continue));}
 assert(host.rewards==1&&host.snapshots==3&&host.last_chapter==44&&host.last_boss&&host.effects.chapter_disable==0);
 // The native player-life guard must not consume the one-shot if retry/death
 // delays the chapter branch. Legacy mode bypasses that Pointdevice guard.
 player.mode_flags=0x100;host.effects.chapter_disable=1;progress.scene_flags|=0x8000;
 SessionFrameState dead;dead.player_life_state=2;assert(runtime.update(dead)==i32(FrameAction::Continue));assert(host.effects.chapter_disable==1&&host.snapshots==3);
 progress.scene_flags|=0x8000;assert(runtime.update({})==i32(FrameAction::Continue));assert(host.effects.chapter_disable==0&&host.snapshots==4);
 EnemyWorldState world;ItemScoreState item_score;std::string wave;CheckpointHost ch(world);ChapterCheckpoint cp(progress,player,item_score,world,wave,ch);
 progress.chapter=7;progress.run_clock=99;PracticePatchEffects effects;effects.chapter_set=43;effects.extra_boss_chapter_bonus=true;
 assert(cp.capture(7,&effects));assert(progress.chapter==43&&world.current_chapter==43&&progress.run_clock==99&&cp.state().chapter==43);
 assert(effects.chapter_set==-1&&!effects.extra_boss_chapter_bonus&&world.chapter_total==1&&ch.captured_total==0);
 assert(cp.restore());assert(world.chapter_total==0&&progress.chapter==43);
 // Hooks consumed once: the following ordinary snapshot has normal totals
 // and uses the requested chapter for the run-clock comparison.
 progress.run_clock=22;assert(cp.capture(8,&effects));assert(world.chapter_total==0&&progress.chapter==8&&progress.run_clock==0);
 ch.sync=false;effects.chapter_set=99;assert(!cp.capture(9,&effects));assert(effects.chapter_set==99);
 std::cout<<"PASS purple typed owners: BGM transitions, movement range, full history caption, AB scoring, time/bonus, life mapping, reward skip, checkpoint order, replay gating\n";
}
