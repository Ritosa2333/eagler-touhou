#include "FighterActor.hpp"
namespace th075 {
void FighterActor::reset_round(FighterCoreEvents& events){
    controls.current.clear();controls.buffered.clear();controls.bufferTimer=0;
    motion.x=side==1?800:480;motion.y=0;motion.clear();motion.pushX=motion.opponentPush=carryPush=0;
    impulseX=impulseY=0;attackPower=attackModifier=0;
    combat.recoveryTimers.fill(0);combat.armorHits=combat.actionDelay=0;
    combat.spellCharge=combat.spellExtent=0;combat.spirit=combat.previousSpirit=1000;combat.spiritDelay=combat.spiritBreak=0;
    combat.spellTimer=0;combat.spellEnded=0;carryMomentum=0;movement.usedAirDashes=movement.usedAirJumps=0;
    damage.comboHits=damage.comboDamage=0;damage.comboScale=1;
    remainingHits=0;response=0;animation.reset(0);combat.action=0;
    combat.freeze=motion.freeze=0;motion.hitFreeze=combat.opponentStop=contactDelay=0;
    pose.active=false;combat.elapsed=0;motion.facing=side==1?-1:1;
    dialogueFinished=false;notice={};noticePortrait=-1;combat.noticeTimer=0;combat.flashTimer=0;
    bodyPower=damage.fighterPower=1;combat.fade=0;damageDisabled=false;transitionReady=true;
    input.clear();controls.raw.clear();rawInputs.fill(0);events.clear_owned_objects(*this);
    selectedSpell=2;combat.spellPhase=2;combat.spellFrames=combat.spellValue=0;combat.health=combat.maximumHealth=10000;
    training={};trainingSpell=0;scriptCursor={};commandTimers.fill(0);launchParameters.fill(0);
}
void FighterActor::guarded_commands(const FighterActor& opponent,GameMode mode,ReplayRandom& random,FighterActionCallbacks& callbacks){
    if(combat.freeze!=0||opponent.motion.hitFreeze!=0)return;
    if(animation.cursor.action<50)callbacks.movement_commands(*this);
    if(animation.cursor.action>=50&&animation.cursor.action<150&&motion.y>0&&animation.frame)
        movement.aerial_recovery(motion.y,motion.facing,combat.health,animation.frame->flags[0],controls.current,combat.storyOpponent,mode==GameMode::Training,training.recovery!=0,random,animation);
    callbacks.attack_commands(*this);combat.action=animation.cursor.action;
}
void FighterActor::save_collision(){
    collision.response=response;collision.frame=animation.frame;
    const i16 action=animation.cursor.action;collision.sequence=animation.pattern&&action>=0&&u32(action)<animation.pattern->sequences.size()?&animation.pattern->sequences[action]:nullptr;
    collision.x=motion.x;collision.y=motion.y;collision.facing=motion.facing;collision.action=action;
}
void FighterActor::begin_pose(float x,float y,float z){pose={};pose.active=true;pose.offset={x,y,z};}
void FighterActor::set_polar_velocity(float degrees,float speed){motion.velocityX=float(double(angle_cos(degrees))*double(speed));motion.velocityY=float(-double(angle_sin(degrees))*double(speed));}
void FighterActor::bias_camera(const FighterActor& opponent,bool reverse,CameraMotion& camera){
    const float direction=reverse?-1:1;camera.biasX=side==0?-50*direction:50*direction;
    camera.biasY=motion.y<=opponent.motion.y?(side==0?-50:50):0;
}
void FighterActor::begin_spell(GameMode mode,u8 replayMode,FighterCoreEvents& events){
    spellElapsed=0;
    if(combat.storyOpponent&&mode==GameMode::Story){
        combat.spellTimer=9999;combat.spellValue=phaseStartValues[combat.spellPhase];combat.spellFrames=phaseBonuses[combat.spellPhase];
        if(replayMode==1)events.spell_attempt(cardIds[combat.spellPhase]);
    }else{const i32 count=i32(double(combat.spellExtent)*8800.0/900.0);combat.spellTimer=i16(u16(increment_wrapped(count,1201)));perfectSpell=false;}
}
}
