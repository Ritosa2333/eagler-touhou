#pragma once
#include "Types.hpp"
#include <string>
#include <vector>
namespace th08 {
// Explicit field order is shared by the browser bridge and replay metadata.
// Keep this independent of object layout, padding and platform pointer width.
struct PracticeConfig {
    i32 mode=1,stage=0,warp=0,section=0,phase=0,frame=0,dlg=0;
    i64 score=0;
    i32 life=2,bomb=8,power=128,gauge=0,graze=0,point=0,point_total=0,point_stage=0;
    i32 time=0,value=60000,night=0,familiar=0,rank=12,rankLock=0;
    static constexpr u32 word_count=23;
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
    u32 cheats=0;bool assisted=false,familiar_pending=false,everlasting_bgm=false;
    // Everlasting-BGM filter state, mirroring ElBgmTest's statics upstream.
    i32 el_bgm_lock=-1;bool el_bgm_block=false;
    // Advanced Options owns these independently of the in-game F1-F7 flags,
    // exactly like THAdvOptWnd's persistent context in upstream thprac.
    bool all_clear_bonus=false,doswnc=false;
    // TH08's Tab tracker keeps this per-run counter separately from the
    // aggregate captured-spell value stored by the original game.
    u32 tracker_last_spell_captures=0,tracker_dissolve_count=0;
};
// Upstream thprac THPracParam::GetJson()/ReadJson() payload for th08.
std::string practice_replay_json(const PracticeConfig&);
bool practice_replay_parse(const char* json,u32 size,PracticeConfig&);
// The 'USER'/'PRAC' block upstream ReplaySaveParam appends to th08+ replays.
// Empty when the config cannot be serialized (caller saves a vanilla replay).
std::vector<u8> practice_replay_block(const PracticeConfig&);
// Upstream ReplayLoadParam for th08: iterate the plain USER block area after
// the encoded replay's file_size and read the 'PRAC' block. Any failure simply
// means "no practice parameters" and playback falls back to Original.
bool practice_replay_read(const u8* data,u32 size,PracticeConfig&);
// THGuiRep replay-menu lifecycle (State 1/2/3) driven by the title replay
// menu, which already reads the raw replay bytes State(2) inspects.
void practice_replay_menu_reset(PracticeState&);
bool practice_replay_menu_check(PracticeState&,const u8* replay,u32 size);
void practice_replay_menu_activate(PracticeState&);
}
