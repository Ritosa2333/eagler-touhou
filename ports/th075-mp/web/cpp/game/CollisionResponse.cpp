#include "CollisionResponse.hpp"
namespace th075 {
namespace {
i16 narrow(i32 v){const u16 bits=u16(v);i16 result;std::memcpy(&result,&bits,2);return result;}
void consume(AttackImpact& attack,i32 response){attack.response=response;attack.remainingHits=narrow(i32(attack.remainingHits)-1);}
void change_action(FighterImpact& target,i16 action,bool clearActive=false){target.animation.reset(action);target.combat.action=action;if(clearActive)target.active=false;}
void reset_combo(FighterImpact& owner){owner.damage.comboHits=owner.damage.comboDamage=0;owner.damage.comboScale=1;}
void add_charge(FighterCombat& target,i16 amount){target.spellCharge=float(double(amount)+double(target.spellCharge));if(target.spellCharge>2000)target.spellCharge=2000;}
i16 resolve_damage(const AttackImpact& attack,const FighterImpact& owner,const FighterImpact& target,bool story){auto damage=owner.damage;damage.sourcePower=attack.power;damage.sourceModifier=attack.modifier;return damage.resolve(attack.frame.baseDamage,target.target,story);}
void start_hit(AttackImpact& attack,FighterImpact& target){consume(attack,1);attack.freeze=attack.frame.combat[2];target.combat.freeze=attack.frame.combat[3];target.impulseX=attack.frame.motion[0];target.impulseY=attack.frame.motion[1];}
void notify_combo(const FighterImpact& owner,CollisionSignals& signals,bool half=false){signals.combo(owner.damage.comboHits,owner.damage.comboDamage,i32(double(owner.damage.comboScale)*1000.0/(half?2.0:1.0)));}
void hit_feedback(AttackImpact& attack,FighterImpact& owner,FighterImpact& target,CollisionSignals& signals){add_charge(owner.combat,attack.frame.combat[6]);target.movement.recoveryDelay=attack.frame.combat[1];target.motion.update_facing(target.opponent);signals.effect(attack.frame.events[1],attack.facing);signals.sound(attack.frame.events[0]);}
}
bool cancel_special_contact(AttackImpact& attack,FighterImpact& target){
    const u32 a=attack.frame.flags[1],b=target.collisionFlags;
    if(!(((a&0x2000)&&(b&0x8000))||((a&0x4000)&&(b&0x10000))||((a&0x8000)&&(b&0x20000))))return false;
    consume(attack,3);target.response=3;return true;
}
bool graze_contact(AttackImpact& attack,FighterImpact& target,CollisionSignals& signals){
    if((attack.frame.flags[1]&0x40000)||(target.combat.action>=50&&target.combat.action<=149)||(!target.combat.recoveryTimers[2]&&!(target.collisionFlags&0x4000)))return false;
    consume(attack,6);target.combat.spellExtent+=0.5f;if(target.combat.spellExtent>900)target.combat.spellExtent=900;signals.effect(52,attack.facing);signals.sound(37);return true;
}
void apply_guard_impact(AttackImpact& attack,FighterImpact& owner,FighterImpact& target,CollisionSignals& signals){
    consume(attack,2);attack.freeze=attack.frame.combat[4];target.combat.freeze=attack.frame.combat[5];target.impulseX=attack.frame.motion[0];target.impulseY=attack.frame.motion[1];
    if(target.combat.health<=0)target.combat.health=1;if(target.combat.spellTimer>0||target.combat.spellEnded)target.combat.maximumHealth=target.combat.health;
    add_charge(owner.combat,attack.frame.combat[7]);target.motion.update_facing(target.opponent);signals.sound(20);
}
bool reflect_contact(AttackImpact& attack,FighterImpact& owner,FighterImpact& target,CollisionSignals& signals){
    const u32 a=attack.frame.flags[1],b=target.collisionFlags;if(!(((a&2)&&(b&0x200))||((a&4)&&(b&0x400))))return false;
    consume(attack,3);signals.effect(51,target.motion.facing);apply_guard_impact(attack,owner,target,signals);return true;
}
void break_guard(FighterImpact& target,i16 action,CollisionSignals& signals){target.combat.spirit=0;target.combat.spiritBreak=1000;change_action(target,action,true);signals.effect(53,target.motion.facing);signals.sound(38);}
void apply_chip_hit(AttackImpact& attack,FighterImpact& owner,FighterImpact& target,bool story,CollisionSignals& signals){
    if(target.combat.action<50||target.combat.action>142)reset_combo(owner);change_action(target,140,true);start_hit(attack,target);
    owner.damage.comboHits=narrow(i32(owner.damage.comboHits)+1);owner.damage.comboDamage=narrow(i32(owner.damage.comboDamage)+resolve_damage(attack,owner,target,story)/2);
    owner.damage.comboScale=float(double(owner.damage.comboScale)*double(attack.frame.proration));notify_combo(owner,signals,true);
    if(!owner.preventDamage)target.combat.health=narrow(i32(target.combat.health)-resolve_damage(attack,owner,target,story)/2);
    if(target.combat.health<=0)target.combat.health=1;if(target.combat.spellTimer>0||target.combat.spellEnded)target.combat.maximumHealth=target.combat.health;hit_feedback(attack,owner,target,signals);
}
bool apply_ground_hit(AttackImpact& attack,FighterImpact& owner,FighterImpact& target,bool story,bool activeRound,CollisionSignals& signals){
    if(((attack.frame.flags[1]&0x400)&&(target.collisionFlags&0x40))||!(attack.frame.flags[1]&0x100000)||!(target.motion.y<=0))return false;
    if(target.combat.action<50||target.combat.action>142)reset_combo(owner);change_action(target,140,true);start_hit(attack,target);
    const i16 damage=resolve_damage(attack,owner,target,story);if(!owner.preventDamage)target.combat.health=narrow(i32(target.combat.health)-damage);
    if(target.combat.health<=0){if(activeRound){target.combat.health=0;change_action(target,attack.frame.combat[8]?attack.frame.combat[8]:70,true);}else target.combat.health=1;}
    if(target.combat.spellTimer>0||target.combat.spellEnded)target.combat.maximumHealth=target.combat.health;
    else target.combat.maximumHealth=narrow(i32(double(target.combat.maximumHealth)-double(damage)*0.25));
    owner.damage.comboHits=narrow(i32(owner.damage.comboHits)+1);owner.damage.comboDamage=narrow(i32(owner.damage.comboDamage)+damage);owner.damage.comboScale=float(double(owner.damage.comboScale)*double(attack.frame.proration));notify_combo(owner,signals);hit_feedback(attack,owner,target,signals);return true;
}
bool apply_guard_response(GuardResponse response,AttackImpact& attack,FighterImpact& owner,FighterImpact& target,bool story,CollisionSignals& signals){
    if(response==GuardResponse::None)return false;if(response==GuardResponse::ChipHit){apply_chip_hit(attack,owner,target,story,signals);return true;}
    i16 chip=attack.frame.combat[0];const i16 style=i8(u8(attack.frame.events[2]));
    switch(response){
        case GuardResponse::Standing:case GuardResponse::Crouching:{
            if(!target.allowPush)target.combat.spirit=narrow(i32(target.combat.spirit)-attack.frame.baseDamage/4);
            const bool crouch=response==GuardResponse::Crouching;if(target.combat.spirit<=0)break_guard(target,crouch?144:143,signals);else change_action(target,narrow(style+(crouch?154:150)));
            signals.effect(50,target.motion.facing);break;
        }
        case GuardResponse::WrongStanding:case GuardResponse::WrongCrouching:{
            target.combat.spirit=narrow(i32(target.combat.spirit)-attack.frame.baseDamage/2);if(attack.frame.flags[1]&0x80000)target.combat.spirit=0;
            const bool crouch=response==GuardResponse::WrongCrouching;if(target.combat.spirit<=0)break_guard(target,crouch?144:143,signals);else change_action(target,narrow(style+(crouch?154:150)));
            signals.effect(51,target.motion.facing);break;
        }
        case GuardResponse::EmptyStanding:case GuardResponse::EmptyCrouching:chip=narrow(i32(chip)+attack.frame.baseDamage/10);change_action(target,narrow(style+(response==GuardResponse::EmptyCrouching?163:159)));signals.effect(51,target.motion.facing);break;
        case GuardResponse::Air:target.combat.spirit=narrow(i32(target.combat.spirit)-attack.frame.baseDamage/4);signals.effect(50,target.motion.facing);if(target.combat.spirit<=0)break_guard(target,145,signals);else change_action(target,158);break;
        default:chip=0;break;
    }
    if(!owner.preventDamage)target.combat.health=narrow(i32(double(target.combat.health)-double(chip)*double(owner.bodyPower)*double(owner.damage.fighterPower)));
    apply_guard_impact(attack,owner,target,signals);return true;
}
}

namespace th075 {
namespace {
void award_hit_score(const AttackImpact& attack,FighterImpact& owner,bool story){
    constexpr i32 points[4]={100,200,300,500};const u32 style=u8(attack.frame.events[2]);if(style<4)owner.score=increment_wrapped(owner.score,points[style]);
    if(story&&owner.playerSide==0){auto& r=owner.records;if(r.maximumDamage<owner.damage.comboDamage)r.maximumDamage=owner.damage.comboDamage;if(r.maximumHits<owner.damage.comboHits)r.maximumHits=owner.damage.comboHits;if(r.sessionCombo<owner.damage.comboHits)r.sessionCombo=owner.damage.comboHits;if(r.highScore<u32(owner.score))r.highScore=u32(owner.score);}
}
void clamp_hit_health(AttackImpact& attack,FighterImpact& target,bool activeRound){
    if(target.combat.health<1){if(activeRound){target.combat.health=0;change_action(target,attack.frame.combat[8]?attack.frame.combat[8]:70,true);}else target.combat.health=1;}
    if(target.combat.spellTimer>0||target.combat.spellEnded)target.combat.maximumHealth=target.combat.health;
}
void finish_combo(const AttackImpact& attack,FighterImpact& owner,CollisionSignals& signals){owner.damage.comboHits=narrow(i32(owner.damage.comboHits)+1);owner.damage.comboScale=float(double(owner.damage.comboScale)*double(attack.frame.proration));notify_combo(owner,signals);}
void fractional_damage(i16 damage,double multiplier,bool counter,FighterImpact& owner,FighterImpact& target,bool armor){
    const bool halves=armor&&!counter;
    if(counter)owner.damage.comboDamage=narrow(i32(double(owner.damage.comboDamage)+double(damage)*multiplier));
    else owner.damage.comboDamage=narrow(i32(owner.damage.comboDamage)+(halves?damage/2:damage));
    if(owner.preventDamage)return;
    if(counter)target.combat.health=narrow(i32(double(target.combat.health)-double(damage)*multiplier));
    else target.combat.health=narrow(i32(target.combat.health)-(halves?damage/2:damage));
    if(target.combat.spellTimer>0){
        if(counter)target.combat.maximumHealth=narrow(i32(double(target.combat.maximumHealth)-double(damage)*multiplier));
        else target.combat.maximumHealth=narrow(i32(target.combat.maximumHealth)-(halves?damage/2:damage));
    }else{
        const double fraction=(armor?0.75:0.8)-double(target.combat.spirit)*(armor?0.25:0.2)/1000.0;
        double shadow=double(damage)*(counter?1.25:1.0);shadow*=fraction;if(armor)shadow/=2.0;
        target.combat.maximumHealth=narrow(i32(double(target.combat.maximumHealth)-shadow));
    }
}
}
void apply_counter_reaction(AttackImpact& attack,FighterImpact& target,CollisionSignals& signals){
    switch(target.combat.action){
        case 50:case 51:case 52:case 56:case 57:case 58:case 62:case 63:case 64:case 70:case 84:change_action(target,narrow(i32(target.combat.action)+1));break;
        case 71:case 72:case 74:case 77:target.impulseX=float(double(target.impulseX)*1.25);target.impulseY=float(double(target.impulseY)*1.25);target.movement.recoveryDelay=narrow(i32(target.movement.recoveryDelay)*2);break;
        case 73:target.movement.recoveryDelay=32767;change_action(target,88);break;
        case 75:case 81:case 83:case 86:case 87:target.movement.recoveryDelay=narrow(i32(target.movement.recoveryDelay)*2);break;
    }
    signals.sound(25);signals.effect(54,attack.facing);
}
void apply_normal_hit(AttackImpact& attack,FighterImpact& owner,FighterImpact& target,bool story,bool activeRound,CollisionSignals& signals){
    attack.response=1;if(target.combat.action<50||target.combat.action>142)reset_combo(owner);target.active=false;
    const i16 style=i8(u8(attack.frame.events[2]));
    if(target.motion.y>0||(target.collisionFlags&4))change_action(target,attack.frame.combat[8]?attack.frame.combat[8]:70);
    else if(attack.frame.combat[9])change_action(target,attack.frame.combat[9]);
    else if(target.collisionFlags&1)change_action(target,narrow(style+((attack.frame.flags[1]&1)?56:50)));
    else if(target.collisionFlags&2)change_action(target,narrow(style+62));
    start_hit(attack,target);add_charge(owner.combat,attack.frame.combat[6]);const i16 damage=resolve_damage(attack,owner,target,story);
    target.combat.spellCharge=float(double(damage)*0.5+double(target.combat.spellCharge));if(target.combat.spellCharge>2000)target.combat.spellCharge=2000;
    target.movement.recoveryDelay=(attack.frame.flags[1]&0x100)?15000:narrow(i32(double(attack.frame.combat[1])*double(owner.damage.comboScale)));
    const bool counter=(attack.frame.flags[1]&0x400)&&(target.collisionFlags&0x40);fractional_damage(damage,counter?1.25:1.0,counter,owner,target,false);if(counter)apply_counter_reaction(attack,target,signals);
    clamp_hit_health(attack,target,activeRound);finish_combo(attack,owner,signals);target.motion.update_facing(target.opponent);signals.effect(attack.frame.events[1],attack.facing);signals.sound(attack.frame.events[0]);award_hit_score(attack,owner,story);
}
bool apply_armor_hit(AttackImpact& attack,FighterImpact& owner,FighterImpact& target,bool story,bool activeRound,CollisionSignals& signals){
    if((attack.frame.flags[1]&0x20)||!((target.collisionFlags&0x80)||((target.collisionFlags&0x100)&&target.combat.armorHits>0)))return false;
    if(target.combat.armorHits>0)--target.combat.armorHits;if(target.combat.actionDelay==0)reset_combo(owner);start_hit(attack,target);attack.response=7;target.combat.actionDelay=15;target.combat.flashTimer=8;
    target.movement.recoveryDelay=(attack.frame.flags[1]&0x100)?30000:narrow(i32(double(attack.frame.combat[1])*double(owner.damage.comboScale)));
    const i16 damage=resolve_damage(attack,owner,target,story);const bool counter=(attack.frame.flags[1]&0x400)&&(target.collisionFlags&0x40);
    fractional_damage(damage,counter?1.25/2.0:0.5,counter,owner,target,true);if(counter)apply_counter_reaction(attack,target,signals);
    clamp_hit_health(attack,target,activeRound);finish_combo(attack,owner,signals);add_charge(owner.combat,attack.frame.combat[6]);
    target.combat.spellCharge=float(double(resolve_damage(attack,owner,target,story))*0.25+double(target.combat.spellCharge));if(target.combat.spellCharge>2000)target.combat.spellCharge=2000;
    signals.effect(attack.frame.events[1],attack.facing);signals.sound(attack.frame.events[0]);award_hit_score(attack,owner,story);return true;
}
}
