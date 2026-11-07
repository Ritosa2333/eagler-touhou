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
    PracticeNoSpirits=1<<9,
    PracticeUncapScore=1<<10,
    PracticeAllClearBonus=1<<11,
};
inline constexpr u32 practice_boss_range_shift=12;
inline constexpr u32 practice_boss_range_steps=500;
inline constexpr u32 practice_tool_mask=(1<<21)-1;
inline u32 practice_boss_range(u32 tools) noexcept{return tools>>practice_boss_range_shift;}
inline i32 add_practice_score(i32 score,i32 amount,bool uncapped) noexcept{
    const u64 total=u64(u32(score))+u32(amount);
    const u64 cap=uncapped?0xffffffffu:999999999u;
    return signed_bits(u32(total<cap?total:cap));
}
struct PracticeToolChange {u32 frame, tools;};
struct PracticeSpiritSpawn {
    i32 kind=0;
    float x=0,y=0,angle=0;
    bool force=false,random=false;
    explicit operator bool()const noexcept{return kind!=0;}
    bool valid()const noexcept{
        return kind>=1&&kind<=17&&x>-192.f&&x<192.f&&y>128.f&&y<448.f&&
            angle>=-3.1415927f&&angle<=3.1415927f&&!(force&&random);
    }
};
struct PracticeSpiritEvent {u32 frame;PracticeSpiritSpawn spawn;};

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
