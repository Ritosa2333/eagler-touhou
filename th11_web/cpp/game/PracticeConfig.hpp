#pragma once
#include "Types.hpp"
#include <vector>
#include <string>
namespace th11 {
// THPracParam in thprac_th11.cpp. stage is zero-based in menu/replay JSON.
// power uses the upstream GUI's twentieth-level units (80, or 96 for MA).
struct PracticeConfig {
    i32 mode=1,stage=0,section=0,phase=0,life=9,life_fragment=0;
    i32 power=80,graze=0,signal=0,value=50000;
    i64 score=0;
    i32 marisa_b_formation=0;
    bool dlg=false;
    void reset(){*this={};mode=life=power=value=0;}
    bool valid()const;
    static constexpr u32 word_count=14;
    bool decode(const double*,u32);
    void encode(double*)const;
};
struct PracticeState {
    bool enabled=false,active=false,replay=false,menu=false,accepted=false;
    PracticeConfig configured,run,replay_candidate;
    bool replay_candidate_valid=false,assisted=false,everlasting_bgm=false,all_clear_bonus=false;
    int warp=0,spell_category=0;
    u32 cheats=0,tracker_misses=0,tracker_bombs=0;
    void clear_run(){active=replay=menu=accepted=replay_candidate_valid=assisted=false;run.reset();replay_candidate.reset();tracker_misses=tracker_bombs=0;}
};
std::string practice_replay_json(const PracticeConfig&);
bool practice_replay_parse(const char*,u32,PracticeConfig&);
std::vector<u8> practice_replay_block(const PracticeConfig&);
bool practice_replay_read(const u8*,u32,PracticeConfig&);
}
