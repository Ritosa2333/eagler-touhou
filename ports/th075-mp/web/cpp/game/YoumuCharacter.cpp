#include "YoumuCharacter.hpp"
#include "YoumuObjects.hpp"
#include "YoumuStory.hpp"
namespace th075 {
void YoumuCharacter::reset(FighterCoreEvents& events){reset_youmu(actor,state,events);}
void YoumuCharacter::recognize_inputs(){actor.command=recognize_youmu_command(actor.controls.history,actor.motion.facing);}
void YoumuCharacter::advance_action(FighterActor& opponent,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){advance_youmu_action(actor,opponent,state,mode,random,replayMode,core,events);}
void YoumuCharacter::advance_character_frame(FighterActor& opponent,GameMode mode,ReplayStreams& random,FighterActionEvents& events){advance_youmu_frame(actor,opponent,state,mode,random,events);}
std::unique_ptr<FighterObject> YoumuCharacter::create_object(){return std::make_unique<YoumuObject>();}
void YoumuCharacter::advance_object(FighterObject& object,FighterActor& opponent,ReplayStreams& random,FighterActionEvents& events){advance_youmu_object(static_cast<YoumuObject&>(object),actor,opponent,state,random,events);}
void YoumuCharacter::initialize_story(FighterActor& opponent,StoryEnvironment& environment){initialize_youmu_story(actor,opponent,state.story,environment);}
void YoumuCharacter::advance_story(FighterActor& opponent,StoryEnvironment& environment,StoryEvents& events){advance_youmu_story(actor,opponent,state.story,state.introWait,environment,events);}
void YoumuCharacter::render_story(u8 stage,ZunGraphics& graphics,const std::array<u32,4>& textures){state.story.render(stage,actor,state.captionWait,graphics,textures);}
void YoumuCharacter::choose_ai_input(const FighterActor& opponent,bool story,ReplayStreams& streams,CharacterAIContext& context){ auto& random=streams;if(story)choose_youmu_story_input(actor,opponent,state,random,context);else choose_youmu_duel_input(actor,opponent,state,random,context);}
void YoumuCharacter::select_commands(const FighterActor& opponent,GameMode mode,ReplayRandom& random){
    struct Callbacks final:FighterActionCallbacks {
        YoumuCharacter& character;const FighterActor& opponent;
        Callbacks(YoumuCharacter& character,const FighterActor& opponent):character(character),opponent(opponent){}
        void movement_commands(FighterActor& a)override{a.movement.select(a.motion,opponent.motion,a.controls.current,a.animation);}
        void attack_commands(FighterActor& a)override{select_youmu_commands(a,opponent,character.state);}
    } callbacks(*this,opponent);
    actor.guarded_commands(opponent,mode,random,callbacks);
}
}
