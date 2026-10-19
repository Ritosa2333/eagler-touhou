#pragma once
#include "FighterCombat.hpp"
namespace th075 {
struct DamageTarget {
    bool storyOpponent=false;i16 snapshotHealth=0;u8 phase=0;
    std::array<i16,12> phaseMaximum{};
};
struct DamageModel {
    float sourcePower=1,sourceModifier=1,fighterPower=1,comboScale=1;
    i16 comboHits=0,comboDamage=0;
    i16 resolve(i16 base,const DamageTarget&,bool storyMode)const;
    void apply_scripted(i16 base,i16 proration,i16 hits,FighterCombat& defender,const DamageTarget&,bool storyMode);
};
}

