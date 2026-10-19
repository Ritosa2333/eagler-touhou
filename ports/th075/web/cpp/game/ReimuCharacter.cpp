#include "ReimuCharacter.hpp"
#include "ReimuInputs.hpp"
#include "ReimuAttacks.hpp"
#include "ReimuRanged.hpp"
#include "ReimuSpells.hpp"
#include "ReimuAI.hpp"
#include "ReimuStory.hpp"
#include "ReimuObjects.hpp"
namespace th075 {
void ReimuCharacter::reset(FighterCoreEvents& events){
    state.recovery={};state.commands={};state.frame={};state.actionCounters.fill(0);
    state.orbitAngle=state.volleyMode=0;actor.knockdownTime=0;actor.commandTimers.fill(0);
    actor.reset_round(events);state.introWait=state.captionWait=0;
}
void ReimuCharacter::recognize_inputs(){actor.command=recognize_reimu_command(actor.controls.history,actor.motion.facing);}
void ReimuCharacter::advance_action(FighterActor& opponent,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){
    if(actor.combat.freeze!=0||opponent.combat.opponentStop!=0)return;
    if(advance_reimu_recovery(actor,opponent,state.recovery,mode,events)||
       advance_reimu_melee(actor,opponent,state.recovery,events)||
       advance_reimu_shooting(actor,opponent,state.recovery,events)||
       advance_reimu_ranged(actor,opponent,state.recovery,random,events)||
       advance_reimu_special_melee(actor,opponent,state.recovery,events)||
       advance_reimu_spells(actor,opponent,state.recovery,mode,state.difficulty,replayMode,random,core,events))return;
    const auto result=actor.animation.advance();if(result==actor.animation.cursor.frameCount)actor.select_action(0);
}
void ReimuCharacter::advance_character_frame(FighterActor& opponent,GameMode mode,ReplayStreams& random,FighterActionEvents& events){advance_reimu_frame(actor,opponent,state.frame,events);}
std::unique_ptr<FighterObject> ReimuCharacter::create_object(){return std::make_unique<ReimuObject>();}
void ReimuCharacter::advance_object(FighterObject& p,FighterActor& other,ReplayStreams& random,FighterActionEvents& events){advance_reimu_object(static_cast<ReimuObject&>(p),actor,other,state,random,events);}
void ReimuCharacter::initialize_story(FighterActor& other,StoryEnvironment& env){initialize_reimu_story(actor,other,state.story,env);}
void ReimuCharacter::advance_story(FighterActor& other,StoryEnvironment& env,StoryEvents& events){advance_reimu_story(actor,other,state.story,state.introWait,env,events);}
void ReimuCharacter::render_story(u8 stage,ZunGraphics& graphics,const std::array<u32,4>& textures){state.story.render(stage,actor,state.captionWait,graphics,textures);}
void ReimuCharacter::choose_ai_input(const FighterActor& other,bool story,ReplayStreams& streams,CharacterAIContext& context){ auto& random=streams.random;
    if(story)choose_reimu_story_input(actor,other,random,context);else choose_reimu_duel_input(actor,other,random,context);
}
void ReimuCharacter::select_commands(const FighterActor& opponent,GameMode mode,ReplayRandom& random){
    struct Callbacks final:FighterActionCallbacks {
        ReimuCharacter& character;const FighterActor& opponent;
        Callbacks(ReimuCharacter& character,const FighterActor& opponent):character(character),opponent(opponent){}
        void movement_commands(FighterActor& a)override{a.movement.select(a.motion,opponent.motion,a.controls.current,a.animation);}
        void attack_commands(FighterActor& a)override{select_reimu_commands(a,opponent,character.state.commands);}
    } callbacks(*this,opponent);
    actor.guarded_commands(opponent,mode,random,callbacks);
}
}
