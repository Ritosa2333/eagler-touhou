#include "RemiliaCharacter.hpp"
#include "RemiliaObjects.hpp"
#include "RemiliaStory.hpp"
namespace th075 {
void RemiliaCharacter::reset(FighterCoreEvents& events){reset_remilia(actor,state,events);}
void RemiliaCharacter::recognize_inputs(){actor.command=recognize_remilia_command(actor.controls.history,actor.motion.facing);}
void RemiliaCharacter::advance_action(FighterActor& opponent,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){advance_remilia_action(actor,opponent,state,mode,random,replayMode,core,events);}
void RemiliaCharacter::advance_character_frame(FighterActor& opponent,GameMode mode,ReplayStreams& random,FighterActionEvents& events){advance_remilia_frame(actor,opponent,state,random,events);}
std::unique_ptr<FighterObject> RemiliaCharacter::create_object(){return std::make_unique<RemiliaObject>();}
void RemiliaCharacter::advance_object(FighterObject& object,FighterActor& opponent,ReplayStreams& random,FighterActionEvents& events){advance_remilia_object(static_cast<RemiliaObject&>(object),actor,opponent,state,random,events);}
void RemiliaCharacter::initialize_story(FighterActor& opponent,StoryEnvironment& environment){initialize_remilia_story(actor,opponent,state.story,environment);}
void RemiliaCharacter::advance_story(FighterActor& opponent,StoryEnvironment& environment,StoryEvents& events){advance_remilia_story(actor,opponent,state.story,state.introWait,environment,events);}
void RemiliaCharacter::render_story(u8 stage,ZunGraphics& graphics,const std::array<u32,4>& textures){state.story.render(stage,actor,state.captionWait,graphics,textures);}
void RemiliaCharacter::choose_ai_input(const FighterActor& opponent,bool story,ReplayStreams& streams,CharacterAIContext& context){ auto& random=streams.random;if(story)choose_remilia_story_input(actor,opponent,state,random,context);else choose_remilia_duel_input(actor,opponent,state,random,context);}
void RemiliaCharacter::select_commands(const FighterActor& opponent,GameMode mode,ReplayRandom& random){
    struct Callbacks final:FighterActionCallbacks {
        RemiliaCharacter& character;const FighterActor& opponent;
        Callbacks(RemiliaCharacter& character,const FighterActor& opponent):character(character),opponent(opponent){}
        void movement_commands(FighterActor& a)override{a.movement.select(a.motion,opponent.motion,a.controls.current,a.animation);}
        void attack_commands(FighterActor& a)override{select_remilia_commands(a,opponent,character.state);}
    } callbacks(*this,opponent);
    actor.guarded_commands(opponent,mode,random,callbacks);
}
}
