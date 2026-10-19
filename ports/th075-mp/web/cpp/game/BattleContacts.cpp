#include "BattleContacts.hpp"
namespace th075 {
void BattleContacts::advance(const std::array<FighterActor*,2>& fighters,const std::array<OwnedObjects*,2>& objects,GameMode mode,BattlePhase phase,ReplayStreams& replay,BattleContactSignals& signals){
    std::array<DamageTarget,2> damageTargets;
    for(u32 side=0;side<2;side++){
        auto& actor=*fighters[side];actor.save_collision();actor.collisionHealth=actor.combat.health;
        damageTargets[side]={actor.combat.storyOpponent,actor.collisionHealth,actor.combat.spellPhase,actor.phaseMaximum};
        for(u8 layer=0;layer<2;layer++)for(u32 i=0;i<objects[side]->size(layer);i++)objects[side]->get(layer,i).save_collision();
    }
    // Clashes run before any damage contact. Snapshot responses let all clash
    // pairs use the same starting state, while the hit counters remain live.
    for(u8 layer=0;layer<2;layer++)for(u32 i=0;i<objects[0]->size(layer);i++){
        auto& a=objects[0]->get(layer,i);if(!a.animation.frame||!a.collision.sequence)continue;
        for(u8 otherLayer=0;otherLayer<2;otherLayer++)for(u32 j=0;j<objects[1]->size(otherLayer);j++){
            auto& b=objects[1]->get(otherLayer,j);if(!b.animation.frame||!b.collision.sequence)continue;
            ProjectileCollisionView av{*a.animation.frame,a.animation.pattern->sequences[a.animation.cursor.action],*a.collision.sequence,a.motion,a.collision.response,a.response,a.hits,a.attackPower,a.attackModifier};
            ProjectileCollisionView bv{*b.animation.frame,b.animation.pattern->sequences[b.animation.cursor.action],*b.collision.sequence,b.motion,b.collision.response,b.response,b.hits,b.attackPower,b.attackModifier};
            resolve_projectile_contact(bounds,av,bv);
        }
    }
    const auto fighterImpact=[&](u32 side){
        auto& a=*fighters[side];a.combat.action=a.animation.cursor.action;
        FighterImpact view{a.motion,fighters[1-side]->motion,a.combat,a.animation,a.movement,a.damage,damageTargets[side],a.response,a.pose.active,a.allowPush,a.impulseX,a.impulseY,a.score,records};
        view.playerSide=a.side;view.bodyPower=a.bodyPower;view.preventDamage=a.damageDisabled;return view;
    };
    const auto hit=[&](u32 side,const CollisionSnapshot& snapshot,const FighterMotion& motion,i32& response,i16& hits,i16& freeze,float power,float modifier){
        const u32 otherSide=1-side;auto& target=*fighters[otherSide];if(!snapshot.frame||!target.collision.frame)return;
        auto ownerView=fighterImpact(side),targetView=fighterImpact(otherSide);
        DefenseView defense{target.motion,fighters[side]->motion,target.combat,target.controls.current};
        defense.cpuControlled=target.combat.storyOpponent;defense.training=mode==GameMode::Training;defense.trainingGuard=target.training.counter;
        defense.difficulty=difficulty;defense.guardEstablished=target.guardEstablished;
        AttackImpact attack{*snapshot.frame,response,hits,freeze,motion.facing,power,modifier};
        resolve_fighter_contact(bounds,attack,{motion,snapshot.response},ownerView,targetView,{*target.collision.frame,target.animation.frame?target.animation.frame->flags[0]:0},defense,replay,mode==GameMode::Story,phase==BattlePhase::Active,signals);
    };
    for(u32 side=0;side<2;side++){
        signals.select_side(u8(side));
        for(u8 layer=0;layer<2;layer++)for(u32 i=0;i<objects[side]->size(layer);i++){
            auto& a=objects[side]->get(layer,i);
            // The original re-saves each object here: a clash response above
            // prevents it from also damaging a fighter during this frame.
            a.save_collision();hit(side,a.collision,a.motion,a.response,a.hits,a.freeze,a.bodyPower,a.sourceModifier);
        }
    }
    for(u32 side=0;side<2;side++){
        signals.select_side(u8(side));auto& a=*fighters[side];hit(side,a.collision,a.motion,a.response,a.remainingHits,a.combat.freeze,a.bodyPower,a.damage.sourceModifier);
    }
    for(u32 side=0;side<2;side++){signals.select_side(u8(side));const auto& a=*fighters[side];signals.combo(a.damage.comboHits,a.damage.comboDamage,i32(double(a.damage.comboScale)*1000));}
}
}
