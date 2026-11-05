#include "AliceCharacter.hpp"
#include "AliceObjects.hpp"
#include "AliceStory.hpp"
namespace th075 {
void AliceCharacter::reset(FighterCoreEvents& events){reset_alice(actor,state,events);}
void AliceCharacter::recognize_inputs(){actor.command=recognize_alice_command(actor.controls.history,actor.motion.facing);}
void AliceCharacter::advance_action(FighterActor& opponent,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){advance_alice_action(actor,opponent,state,mode,random,replayMode,core,events);}
void AliceCharacter::advance_character_frame(FighterActor& opponent,GameMode mode,ReplayStreams& random,FighterActionEvents& events){advance_alice_frame(actor,opponent,state,events);}
std::unique_ptr<FighterObject> AliceCharacter::create_object(){return std::make_unique<AliceObject>();}
void AliceCharacter::advance_object(FighterObject& object,FighterActor& opponent,ReplayStreams& random,FighterActionEvents& events){advance_alice_object(static_cast<AliceObject&>(object),actor,opponent,state,random,events);}
void AliceCharacter::initialize_story(FighterActor& opponent,StoryEnvironment& environment){initialize_alice_story(actor,opponent,state.story,environment);}
void AliceCharacter::advance_story(FighterActor& opponent,StoryEnvironment& environment,StoryEvents& events){advance_alice_story(actor,opponent,state.story,state.introWait,environment,events);}
void AliceCharacter::render_story(u8 stage,ZunGraphics& graphics,const std::array<u32,4>& textures){state.story.render(stage,actor,state.captionWait,graphics,textures);}
void AliceCharacter::choose_ai_input(const FighterActor& opponent,bool story,ReplayStreams& streams,CharacterAIContext& context){ auto& random=streams.random;if(story)choose_alice_story_input(actor,opponent,state,random,context);else choose_alice_duel_input(actor,opponent,state,random,context);}
void AliceCharacter::select_commands(const FighterActor& opponent,GameMode mode,ReplayRandom& random){
    struct Callbacks final:FighterActionCallbacks {
        AliceCharacter& character;const FighterActor& opponent;
        Callbacks(AliceCharacter& character,const FighterActor& opponent):character(character),opponent(opponent){}
        void movement_commands(FighterActor& a)override{a.movement.select(a.motion,opponent.motion,a.controls.current,a.animation);}
        void attack_commands(FighterActor& a)override{select_alice_commands(a,opponent,character.state);}
    } callbacks(*this,opponent);
    actor.guarded_commands(opponent,mode,random,callbacks);
}
}
