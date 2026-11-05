#pragma once
#include "RoundRules.hpp"
#include "FighterMovement.hpp"
#include "InputMapping.hpp"
#include "SpriteRenderer.hpp"
#include "DamageModel.hpp"
#include "CameraMotion.hpp"
namespace th075 {
struct FighterActor;
struct FighterCoreEvents {
    virtual ~FighterCoreEvents()=default;
    virtual void clear_owned_objects(FighterActor&)=0;
    virtual void spell_attempt(u8 card)=0;
    virtual void reset_hud_alpha()=0;
};
struct FighterActionCallbacks {
    virtual ~FighterActionCallbacks()=default;
    virtual void movement_commands(FighterActor&)=0;
    virtual void attack_commands(FighterActor&)=0;
};
struct TrainingOptions {u8 guard=0,spell=0,recovery=0,counter=0,direction=0,position=0;};
struct FighterNotice {u8 mode=0,brightness=0,visible=1,transition=0,dialogueVisible=0;i16 duration=25,level=0;};
struct StoryDialogueCursor {i16 label=0,branches=0,record=0,part=5;};
struct FighterScriptCursor {u8 waiting=0;i16 musicDelay=-1,musicId=0;};
struct CollisionSnapshot {
    i32 response=0;const PatternFrame* frame=nullptr;const PatternSequence* sequence=nullptr;
    float x=0,y=0;i8 facing=1;i16 action=0;
};
// One named state owner is shared by a character's actions, damage, replay and
// render callbacks. Native-layout byte fixtures exist only in comparison tests.
struct FighterActor:RoundFighterState {
    u8 side=0;FighterAnimation animation;FighterControls controls;FighterMovement movement;InputMapping input;
    SpritePose pose;FrameVisual effectVisual;SpriteTrails trails;DamageModel damage;CollisionSnapshot collision;
    i32 response=0;i16 remainingHits=0,contactDelay=0,collisionHealth=0;bool guardEstablished=false;
    float attackPower=0,attackModifier=0,impulseX=0,impulseY=0,bodyPower=1,carryPush=0;
    bool allowPush=false;u8 carryMomentum=0,command=0,fadeKind=0;i16 spellElapsed=0;
    std::array<i32,12> phaseStartValues{};std::array<u8,13> phaseVariants{};
    std::array<i16,3> commandTimers{};std::array<i16,2> spellChoices{};
    std::array<float,4> launchParameters{};i16 knockdownTime=0;i32 stageClearBonus=0;
    StoryDialogueCursor dialogue;i8 portraitMaximum=10;TrainingOptions training;FighterNotice notice;FighterScriptCursor scriptCursor;
    void reset_round(FighterCoreEvents&);
    void guarded_commands(const FighterActor& opponent,GameMode,ReplayRandom&,FighterActionCallbacks&);
    void save_collision();
    void reset_command_history(){controls.history.reset(controls.current.held());}
    void begin_pose(float x,float y,float z);
    void set_polar_velocity(float degrees,float speed);
    void bias_camera(const FighterActor& opponent,bool reverse,CameraMotion&);
    void begin_spell(GameMode,u8 replayMode,FighterCoreEvents&);
    void select_action(i16 next){animation.reset(next);combat.action=animation.cursor.action;}
};
}
