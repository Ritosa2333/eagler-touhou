#include "MeilingCharacter.hpp"
#include "MeilingObjects.hpp"
#include "MeilingStory.hpp"
namespace th075 {
void MeilingCharacter::reset(FighterCoreEvents& events){reset_meiling(actor,state,events);}
void MeilingCharacter::recognize_inputs(){actor.command=recognize_meiling_command(actor.controls.history,actor.motion.facing);}
void MeilingCharacter::advance_action(FighterActor& opponent,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){advance_meiling_action(actor,opponent,state,mode,random,replayMode,core,events);}
void MeilingCharacter::advance_character_frame(FighterActor& opponent,GameMode mode,ReplayStreams& random,FighterActionEvents& events){advance_meiling_frame(actor,opponent,state,random,events);}
std::unique_ptr<FighterObject> MeilingCharacter::create_object(){return std::make_unique<MeilingObject>();}
void MeilingCharacter::advance_object(FighterObject& object,FighterActor& opponent,ReplayStreams& random,FighterActionEvents& events){advance_meiling_object(static_cast<MeilingObject&>(object),actor,opponent,state,random,events);}
void MeilingCharacter::initialize_story(FighterActor& opponent,StoryEnvironment& environment){initialize_meiling_story(actor,opponent,state.story,environment);}
void MeilingCharacter::advance_story(FighterActor& opponent,StoryEnvironment& environment,StoryEvents& events){advance_meiling_story(actor,opponent,state.story,state.introWait,environment,events);}
void MeilingCharacter::render_story(u8 stage,ZunGraphics& graphics,const std::array<u32,4>& textures){state.story.render(stage,actor,state.captionWait,graphics,textures);}
void MeilingCharacter::choose_ai_input(const FighterActor& opponent,bool story,ReplayStreams& streams,CharacterAIContext& context){ auto& random=streams.random;if(story)choose_meiling_story_input(actor,opponent,state,random,context);else choose_meiling_duel_input(actor,opponent,state,random,context);}
void MeilingCharacter::select_commands(const FighterActor& opponent,GameMode mode,ReplayRandom& random){
    struct Callbacks final:FighterActionCallbacks {
        MeilingCharacter& character;const FighterActor& opponent;
        Callbacks(MeilingCharacter& character,const FighterActor& opponent):character(character),opponent(opponent){}
        void movement_commands(FighterActor& a)override{a.movement.select(a.motion,opponent.motion,a.controls.current,a.animation);}
        void attack_commands(FighterActor& a)override{select_meiling_commands(a,opponent,character.state);}
    } callbacks(*this,opponent);
    actor.guarded_commands(opponent,mode,random,callbacks);
}
}
