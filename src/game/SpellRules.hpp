#pragma once
#include "EnemyPhaseRules.hpp"
#include <bit>
namespace th128::spell {
struct Result {int card=-1,notice=9,bonus=0,rawScore=0;bool captured=false;};
inline bool begin(enemy::PhaseSpell& state,int id,int limit,int difficulty,int stageDepth) noexcept {
    if(id<0||id>=256||limit<=300||limit>65535||difficulty<0||difficulty>3||stageDepth<1||stageDepth>3)return false;
    state.flags=(state.flags&~0xf0u)|7u;if(state.playerMode)state.flags|=0x40u;
    state.age=0;state.id=id;state.limit=limit;state.baseBonus=(difficulty+stageDepth)*100000;state.value80=std::uint32_t(state.baseBonus);return true;
}
inline bool begin(enemy::PhaseSpell& state,int id,int limit,int difficulty) noexcept {
    return begin(state,id,limit,difficulty,1);
}
inline bool beginExtraStage16Rank4(enemy::PhaseSpell& state,int id,int limit) noexcept {
    if(id<240||id>249||limit<=300||limit>65535)return false;
    state.flags=(state.flags&~0xf0u)|7u;if(state.playerMode)state.flags|=0x40u;
    state.age=0;state.id=id;state.limit=limit;state.baseBonus=500000;state.value80=500000;return true;
}
inline bool tick(enemy::PhaseSpell& state) noexcept {
    if(!(state.flags&1u))return true;
    if(state.age<0||state.age>1000000||state.limit<=300||state.baseBonus<0)return false;
    int value=state.baseBonus;
    if(state.age>=300&&!(state.flags&0x10u)){
        const auto product=std::uint32_t(state.baseBonus*3/4)*std::uint32_t(state.age-300);
        value-=std::bit_cast<std::int32_t>(product)/(state.limit-300);value-=value%10;
    }
    state.value80=std::uint32_t(value);++state.age;if(state.playerMode==0)state.flags&=~0x40u;return true;
}
inline Result end(enemy::PhaseSpell& state) noexcept {
    Result result;result.card=state.id;
    if(state.flags&1u){result.captured=state.flags&2u;result.notice=result.captured?(state.flags&4u?7:8):9;result.bonus=result.captured?std::bit_cast<std::int32_t>(state.value80):0;result.rawScore=result.bonus/10;state.flags&=~0x41u;}
    return result;
}
inline void frozen(enemy::PhaseSpell& state) noexcept {
    if(!(state.flags&1u))return;
    if(state.age>=60)state.flags&=~0x44u;else if(state.playerMode)state.flags|=0x40u;
}
}
