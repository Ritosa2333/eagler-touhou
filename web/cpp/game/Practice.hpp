#pragma once
#include "Types.hpp"
#include <array>
#include <string>
namespace th17 {
class EclProgram;
enum PracticeTool : u32 {
    PracticeInvincible=1<<0,
    PracticeInfiniteLives=1<<1,
    PracticeInfiniteBombs=1<<2,
    PracticeInfinitePower=1<<3,
    PracticeTimeLock=1<<4,
    PracticeAutoBomb=1<<5,
    PracticeInfiniteRoaring=1<<6,
    PracticeNoHyper=1<<7,
    PracticeEnemyInvincible=1<<8,
};
inline constexpr u32 practice_tool_mask=(1<<9)-1;
struct PracticeToolChange {u32 frame, tools;};

struct PracticePatch {
    i32 stage=0, section=0, phase=0;
    bool dialogue=false, keep_drops=false;
    std::array<i32,3> keiki_offsets{};
    std::array<i32,5> spirits{};
    bool valid(i32 difficulty,i32 spell)const noexcept;
    explicit operator bool()const noexcept{return stage!=0;}
};

struct PracticeSettings {
    PracticePatch patch;
    i32 lives=8, life_pieces=0, bombs=8, bomb_pieces=0, power=400, value=10000, graze=0;
    i64 score=0;
};

inline constexpr std::array<i32,7> practice_waves{4,5,6,6,6,4,8};
i32 practice_phase_count(i32 section,i32 spell)noexcept;
i32 practice_bgm(i32 section)noexcept;
bool patch_practice_ecl(EclProgram&,const PracticePatch&,i32 difficulty,i32 spell,std::string&);
}
