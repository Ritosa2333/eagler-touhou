#pragma once
#include "Types.hpp"
#include "PracticeNativeHooks.hpp"
#include <eagler/thprac/PracticeInput.hpp>
#include "Types.hpp"
#include <eagler/thprac/PracticeSpeed.hpp>
#include "PracticeCadence.hpp"
#include <string>
#include <vector>
#include <functional>
namespace th15 {
enum PracticeCheat:u32 {PracticeInvincible=1u,PracticeLives=2u,PracticeBombs=4u,PracticePower=8u,PracticeTime=16u,PracticeAutoBomb=32u,PracticeBgm=64u,PracticeEnemyInvincible=128u};
class EclProgram;
// THPracParam from purple thprac_th15.cpp. Stage is zero-based; score
// is display units, power is hundredths, value is GUI units (native *100).
struct PracticeConfig {
 i32 mode=1,stage=0,section=0,phase=0;
 i64 score=0;
 i32 life=8,life_fragment=0,bomb=8,bomb_fragment=0,power=400,value=10000,graze=0,reisen_shield=0;
 float doremy_normal_1_phase=0,enhanced_para=0;
 bool dlg=false;
 void reset()noexcept{*this={};mode=life=bomb=power=value=0;}
 bool valid(i32 difficulty=3)const noexcept;
};
// The four process hooks used by THPatch are one-shot scene owners, not
// permanent changes to ordinary chapter progression.
struct PracticePatchEffects {
 i32 chapter_set=-1,chapter_disable=0;
 bool stars_bgm_sync=false,extra_boss_chapter_bonus=false;
 bool skip_reward()noexcept{if(chapter_disable<=0)return false;--chapter_disable;return true;}
 u32 music_start_offset(bool eligible)noexcept{if(!eligible||!stars_bgm_sync)return 0;stars_bgm_sync=false;return practice_stars_bgm_byte_offset;}
};
// Native 4301e8 sets a flag; RenderLockTimer consumes it once. Multiple
// Boss updates and high-refresh redraws must not multiply this counter.
struct PracticeLockTimer {
 i32 frames=0;bool pending=false;
 void reset()noexcept{frames=0;}
 void observe()noexcept{pending=true;}
 void draw_tick()noexcept{if(pending){frames=wrapping_add(frames,1);pending=false;}}
};
struct PracticeState {
 bool enabled=false,menu=false,accepted=false,cancelled=false,active=false,replay=false;
 bool menu_visible=false,tracker_visible=false,advanced_visible=false,assisted=false;
 bool shooting_down_rate=false,map_inf_life_to_no_continue=true;
 bool force_boss_move_down=false,disable_master_display=false;
 bool all_clear_bonus=false;
 bool show_lock_timer=false;
 bool show_keyboard_monitor=false;
 bool flip_screen_y=false;
 eagler::thprac::PracticeInput input;
 eagler::thprac::PracticeSpeed speed;
 std::function<void(u32)> record_keys;
 mutable PracticeLockTimer lock_timer;
 float boss_move_down_range=practice_boss_range_default;
 i32 warp=0;u32 cheats=0,misses=0,bombs=0;
 i32 ab_result_frames=0;
 PracticeConfig configured,run;
 bool cheat(u32 bit)const noexcept{return enabled&&!replay&&(cheats&bit);}
 // Purple 456397 increments EAX before the original decrement. Its default
 // mapping prevents game over at zero, rather than keeping every stock.
 bool preserve_life(i32 lives)const noexcept{return cheat(PracticeLives)&&(!map_inf_life_to_no_continue||lives==0);}
 void clear_run(){active=replay=menu=accepted=cancelled=assisted=false;run.reset();misses=bombs=0;ab_result_frames=0;lock_timer={};input.reset();}
};
struct PracticeBuffers {std::vector<std::vector<u8>> ecl;PracticePatchEffects effects;};
struct PracticeWrite {u32 file,offset,length;};
bool patch_practice_buffers(PracticeBuffers&,const PracticeConfig&,std::string&,std::vector<PracticeWrite>* writes=nullptr);
// Publishes decoded bytes and the two source-authored routine extents before
// any EclContext is bound. Never reparses unreachable stale instruction tails.
bool patch_practice_program(EclProgram&,const PracticeConfig&,PracticePatchEffects&,std::string&);
std::string practice_replay_json(const PracticeConfig&);
bool practice_replay_parse(const char*,u32,PracticeConfig&);
std::vector<u8> practice_replay_block(const PracticeConfig&);
bool practice_replay_read(const u8*,u32,PracticeConfig&);
// 0 = ordinary replay, 1 = valid PRAC, -1 = malformed PRAC/container.
i32 practice_replay_status(const u8*,u32,PracticeConfig&);
}
