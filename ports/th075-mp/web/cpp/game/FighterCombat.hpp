#pragma once
#include "Types.hpp"
namespace th075 {
struct CombatView {float horizontalOffset=0,verticalOffset=300,spanOffset=0;};
struct FighterCombat {
    i16 action=0,freeze=0,actionDelay=0,armorHits=0;
    std::array<i16,4> recoveryTimers{};
    i16 opponentStop=0;
    float opponentPush=0;
    u32 frameFlags=0;
    bool storyOpponent=false;
    i16 health=10000,maximumHealth=10000;
    float spellCharge=0,spellExtent=0;
    u8 spellEnded=0,spellPhase=0;
    i16 spellTimer=0;
    i32 spellValue=0,spellFrames=0;
    std::array<i32,12> spellDrain{};
    i16 spirit=1000,previousSpirit=1000,spiritDelay=0,spiritBreak=0;
    i32 noticeTimer=0;i8 flashTimer=0;u8 fade=0;
    std::array<float,4> screenBounds{};i16 elapsed=0;
    // Returns true on the original spirit-break recovery sound (se47).
    bool advance(FighterCombat& opponent,bool storyMode,const CombatView&);
};
}

