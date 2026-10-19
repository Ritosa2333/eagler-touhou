#pragma once
#include "FighterCombat.hpp"
#include "FighterAnimation.hpp"
#include "FighterMovement.hpp"
#include "DamageModel.hpp"
#include "GuardSelector.hpp"
namespace th075 {
// Typed views of the live and pre-collision states; no executable-address dispatch.
struct AttackImpact {
    const PatternFrame& frame;i32& response;i16& remainingHits;i16& freeze;i8 facing=1;
    float power=1,modifier=1;
};
struct BattleRecords {i32 maximumDamage=0,maximumHits=0;i16 sessionCombo=0;u32 highScore=0;};
struct FighterImpact {
    FighterMotion& motion;const FighterMotion& opponent;FighterCombat& combat;FighterAnimation& animation;
    FighterMovement& movement;DamageModel& damage;const DamageTarget& target;
    i32& response;bool& active;bool& allowPush;float& impulseX;float& impulseY;i32& score;BattleRecords& records;u8 playerSide=0;
    u32 collisionFlags=0;float bodyPower=1;bool preventDamage=false;
};
struct CollisionSignals {
    virtual ~CollisionSignals()=default;
    virtual void sound(i32 id)=0;virtual void effect(i16 id,i8 facing)=0;
    virtual void combo(i16 hits,i16 damage,i32 scaleUnits)=0;
};
bool cancel_special_contact(AttackImpact&,FighterImpact&);
bool graze_contact(AttackImpact&,FighterImpact&,CollisionSignals&);
void apply_guard_impact(AttackImpact&,FighterImpact& owner,FighterImpact& target,CollisionSignals&);
bool reflect_contact(AttackImpact&,FighterImpact& owner,FighterImpact& target,CollisionSignals&);
void break_guard(FighterImpact&,i16 action,CollisionSignals&);
void apply_chip_hit(AttackImpact&,FighterImpact& owner,FighterImpact& target,bool storyMode,CollisionSignals&);
bool apply_ground_hit(AttackImpact&,FighterImpact& owner,FighterImpact& target,bool storyMode,bool activeRound,CollisionSignals&);
bool apply_guard_response(GuardResponse,AttackImpact&,FighterImpact& owner,FighterImpact& target,bool storyMode,CollisionSignals&);
}

namespace th075 {
void apply_counter_reaction(AttackImpact&,FighterImpact& target,CollisionSignals&);
void apply_normal_hit(AttackImpact&,FighterImpact& owner,FighterImpact& target,bool storyMode,bool activeRound,CollisionSignals&);
bool apply_armor_hit(AttackImpact&,FighterImpact& owner,FighterImpact& target,bool storyMode,bool activeRound,CollisionSignals&);
}
