#include "DamageModel.hpp"
namespace th075 {
static i16 short_bits(i32 value){const u16 bits=u16(value);i16 out;std::memcpy(&out,&bits,2);return out;}
i16 DamageModel::resolve(i16 base,const DamageTarget& target,bool storyMode)const{
    double damage=double(base)*double(sourcePower);damage*=double(sourceModifier);damage*=double(fighterPower);damage*=double(comboScale);
    double bonus=double(target.snapshotHealth)/4.0;
    if(storyMode&&target.storyOpponent){if(target.phase>=target.phaseMaximum.size()||target.phaseMaximum[target.phase]==0)return 0;bonus/=double(target.phaseMaximum[target.phase]);}
    else bonus/=10000.0;
    damage*=bonus+0.75;return short_bits(i32(damage));
}
void DamageModel::apply_scripted(i16 base,i16 proration,i16 hits,FighterCombat& defender,const DamageTarget& target,bool storyMode){
    // Damage uses the previous combo scale; reset happens after resolution.
    const i16 damage=resolve(base,target,storyMode);
    if(defender.action<50||defender.action>142){comboHits=comboDamage=0;comboScale=1;}
    comboDamage=short_bits(i32(comboDamage)+damage);defender.health=short_bits(i32(defender.health)-damage);if(defender.health<1)defender.health=0;
    if(defender.spellTimer<1){const double fraction=0.75-double(defender.spirit)*0.25/1000.0;defender.maximumHealth=short_bits(i32(double(defender.maximumHealth)-double(damage)*fraction));}
    else defender.maximumHealth=short_bits(i32(defender.maximumHealth)-damage);
    if(defender.spellTimer>0||defender.spellEnded!=0)defender.maximumHealth=defender.health;
    comboHits=short_bits(i32(comboHits)+hits);comboScale=float((double(proration)/100.0)*double(comboScale));
}
}


