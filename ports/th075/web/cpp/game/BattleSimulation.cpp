#include "BattleSimulation.hpp"
namespace th075 {
class BattleSimulation::Events final:public FighterCoreEvents,public FighterActionEvents,public BattleContactSignals {
    BattleSimulation& battle;
public:
    u8 side=0;
    explicit Events(BattleSimulation& battle):battle(battle){}
    void select_side(u8 value)override{side=value;}
    void clear_owned_objects(FighterActor& owner)override{for(u8 i=0;i<2;i++)if(&battle.characters[i]->actor==&owner)battle.objects[i].clear();}
    void reset_hud_alpha()override{battle.story.hudAlpha={255,255};}
    void spell_attempt(u8 card)override{battle.signals.spell_attempt(side,card);}
    void sound(i16 id)override{battle.signals.character_sound(side,id);}
    void system_sound(i16 id)override{battle.signals.sound(id);}
    void sound(i32 id)override{battle.signals.sound(i16(id));}
    void combo(i16 hits,i16 damage,i32 scale)override{battle.signals.combo(side,hits,damage,scale);}
    void effect(i16 id,i8 facing)override{
        const auto& bounds=battle.contacts.bounds.intersection;
        battle.effects.spawn(id,float((double(bounds.left)+double(bounds.right))/2.0),float((double(bounds.top)+double(bounds.bottom))/2.0),facing,1);
    }
    void effect(FighterEffectSpace space,i16 id,float x,float y,i8 facing,u8 layer,const std::array<float,4>& parameters,u8 contactDelay,const FrameVisual* visual,u8 alternateDraw,u8 shadow)override{
        if(space==FighterEffectSpace::Shared){battle.effects.spawn(id,x,y,facing,layer,parameters);return;}
        ObjectRequest request;request.action=id;request.x=x;request.y=y;request.facing=facing;request.layer=layer;request.parameters=parameters;request.contactDelay=contactDelay;request.visual=visual;request.alternateDraw=alternateDraw;request.shadow=shadow;
        battle.objects[side].spawn(request);
    }
    void shake(float amplitude)override{battle.story.camera.shake(amplitude);}
    u32& fade_color()override{return battle.story.fadeColor;}
    u8& fade_alpha()override{return battle.story.fadeAlpha;}
    std::array<u8,2>& hud_alpha()override{return battle.story.hudAlpha;}
    u8& completed_stages()override{return battle.story.completedStages;}
    u8& route()override{return battle.story.route;}
    CameraMotion& camera()override{return battle.story.camera;}
};
class BattleSimulation::Objects final:public FighterObjectUpdates {
    BattleSimulation& battle;Events& events;
public:
    Objects(BattleSimulation& battle,Events& events):battle(battle),events(events){}
    void update(FighterObject& object)override{battle.characters[events.side]->advance_object(object,battle.characters[1-events.side]->actor,battle.replay,events);}
};
BattleSimulation::BattleSimulation(const std::array<CharacterController*,2>& characters,const EffectPattern& effectPattern,BattleSimulationSignals& signals)
    :signals(signals),characters(characters),objects{OwnedObjects(characters[0]->actor,*characters[0]),OwnedObjects(characters[1]->actor,*characters[1])},effects(effectPattern,replay.random){}
void BattleSimulation::reset_fighter(u8 side){if(side>=2)return;Events events(*this);events.side=side;characters[side]->set_difficulty(difficulty);characters[side]->reset(events);}
void BattleSimulation::update_inputs(const std::array<ControlCounts,2>& live,const std::array<i32,2>& cancel){
    for(u8 side=0;side<2;side++){
        characters[side]->set_difficulty(difficulty);
        update_character_input(*characters[side],characters[1-side]->actor,live[side],mode,replay,ai,difficulty,cancel[side]);
        restrict_battle_input(characters[side]->actor,rules.phase);
    }
}
void BattleSimulation::separate_bodies(){
    auto& a=characters[0]->actor;auto& b=characters[1]->actor;
    if(!a.animation.frame||!b.animation.frame)return;
    CollisionRectangle first,second;std::memcpy(&first,a.animation.frame->baseBounds.bits.data(),16);std::memcpy(&second,b.animation.frame->baseBounds.bits.data(),16);
    bodies.advance(a.motion,first,b.motion,second);
}
void BattleSimulation::advance_counters(){
    for(auto* character:characters){auto& a=character->actor;a.combat.action=a.animation.cursor.action;a.combat.frameFlags=a.animation.frame?a.animation.frame->flags[0]:0;a.combat.opponentPush=a.motion.opponentPush;}
    for(u8 side=0;side<2;side++)if(characters[side]->actor.combat.advance(characters[1-side]->actor.combat,mode==GameMode::Story,story.camera.combat_view()))signals.sound(47);
    for(auto* character:characters){auto& a=character->actor;a.motion.opponentPush=a.combat.opponentPush;a.combat.opponentStop=a.motion.hitFreeze;if(a.motion.hitFreeze>0)--a.motion.hitFreeze;a.motion.freeze=a.combat.freeze;}
}
void BattleSimulation::advance(StoryEvents& storyEvents){
    if(rules.freezeFrames>0){--rules.freezeFrames;return;}
    Events events(*this);Objects updater(*this,events);
    for(auto* character:characters)character->set_difficulty(difficulty);
    for(u8 side=0;side<2;side++)characters[side]->select_commands(characters[1-side]->actor,mode,replay.random);
    if(rules.phase==BattlePhase::StoryDialogue)characters[0]->advance_story(characters[1]->actor,story,storyEvents);
    for(u8 side=0;side<2;side++){events.side=side;characters[side]->advance_action(characters[1-side]->actor,mode,replay,replayMode,events,events);}
    for(u8 side=0;side<2;side++){events.side=side;objects[side].advance(updater);}
    if(rules.phase!=BattlePhase::StoryDialogue&&rules.phase!=BattlePhase::Intro&&rules.phase!=BattlePhase::RoundRecovery&&rules.phase!=BattlePhase::StoryRecovery){contacts.difficulty=difficulty;contacts.records.highScore=rules.highScore;contacts.advance({&characters[0]->actor,&characters[1]->actor},{&objects[0],&objects[1]},mode,rules.phase,replay,events);rules.highScore=contacts.records.highScore;}
    separate_bodies();
    if(rules.phase!=BattlePhase::StoryDialogue&&mode!=GameMode::Training)rules.detect_knockout(characters[0]->actor,characters[1]->actor,mode,replayMode);
    for(auto* character:characters)character->actor.motion.freeze=character->actor.combat.freeze;
    for(u8 side=0;side<2;side++)characters[side]->actor.motion.advance_position(characters[1-side]->actor.motion);
    advance_counters();
    for(u8 side=0;side<2;side++){events.side=side;characters[side]->advance_character_frame(characters[1-side]->actor,mode,replay,events);}
    effects.advance();signals.advance_stage();
}
}
