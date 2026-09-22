#pragma once
#include "Types.hpp"
#include <string>
#include <vector>
namespace th10 {
// Explicit field order is shared by the browser bridge and replay metadata.
// Keep this independent of object layout, padding and platform pointer width.
// Field set and defaults match thprac_th10.cpp's THPracParam/THGuiPrac.
struct PracticeConfig {
    i32 mode=1,stage=0,warp=0,section=0,phase=0,frame=0,dlg=0;
    i64 score=0;
    i32 life=9,power=100,faith=50000,faith_bar=130,st6_boss9_spd=160;
    i32 real_bullet_sprite=0;
    static constexpr u32 word_count=15;
    // Upstream THPracParam::Reset() clears every field. This is distinct from
    // the initialized Practice-menu defaults above.
    void reset();
    bool decode(const double* words,u32 count);
    void encode(double* words)const;
    bool valid()const;
};
struct PracticeState {
    bool enabled=false,active=false,replay=false,menu=false,accepted=false;
    PracticeConfig configured,run;
    // THGuiRep keeps a candidate replay parameter block separate from the live
    // run. State(2) only inspects the opened replay; State(3) copies the
    // candidate into run when playback is accepted.
    PracticeConfig replay_candidate;bool replay_candidate_valid=false;
    // Cheats are deliberately unavailable during replay and disable saving a
    // recording once used: a changing trainer state isn't a deterministic run.
    u32 cheats=0;bool assisted=false,everlasting_bgm=false;
    // Everlasting-BGM filter state, mirroring ElBgmTest's statics upstream.
    i32 el_bgm_lock=-1;bool el_bgm_block=false;
    // Advanced Options owns this independently of the in-game F1-F6 flags,
    // exactly like THAdvOptWnd's persistent context in upstream thprac.
    bool all_clear_bonus=false;
    // TH10's Tab tracker keeps its own counters (TH10Info) instead of reading
    // the aggregate score fields the original game stores.
    u32 tracker_misses=0,tracker_bombs=0;
    // The real-bullet-sprite toggle has no exact runtime mapping yet; the
    // request is retained here for the Phase D consumer.
    bool real_bullet_sprite=false;
};
// Upstream thprac THPracParam::GetJson()/ReadJson() payload for th10.
std::string practice_replay_json(const PracticeConfig&);
bool practice_replay_parse(const char* json,u32 size,PracticeConfig&);
// The 'USER'/'PRAC' block upstream ReplaySaveParam appends to th10 replays.
// Empty when the config cannot be serialized (caller saves a vanilla replay).
std::vector<u8> practice_replay_block(const PracticeConfig&);
// Upstream ReplayLoadParam for th10: iterate the plain USER block area after
// the encoded replay's file_size and read the 'PRAC' block. Any failure simply
// means "no practice parameters" and playback falls back to Original.
bool practice_replay_read(const u8* data,u32 size,PracticeConfig&);
// THGuiRep replay-menu lifecycle (State 1/2/3) driven by the title replay
// menu, which already reads the raw replay bytes State(2) inspects.
void practice_replay_menu_reset(PracticeState&);
bool practice_replay_menu_check(PracticeState&,const u8* replay,u32 size);
void practice_replay_menu_activate(PracticeState&);
// thprac_th10.cpp:2197 THRepPowerFix. Rewrites the first recorded stage's
// 128-byte option block at offset 0x198 of the plain replay payload from the
// six per-shot tables selected by power (thprac_th10.cpp:2397).
void practice_replay_power_fix(u8* buffer,i32 character,i32 shot_type,i32 power);
}
