#include "YukariCharacter.hpp"
#include "YukariObjects.hpp"
#include "YukariStory.hpp"
namespace th075 {
void YukariCharacter::reset(FighterCoreEvents& events){reset_yukari(actor,state,events);}
void YukariCharacter::recognize_inputs(){actor.command=recognize_yukari_command(actor.controls.history,actor.motion.facing);}
void YukariCharacter::advance_action(FighterActor& opponent,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){advance_yukari_action(actor,opponent,state,mode,random,replayMode,core,events);}
void YukariCharacter::advance_character_frame(FighterActor& opponent,GameMode mode,ReplayStreams& random,FighterActionEvents& events){advance_yukari_frame(actor,opponent,state,random,events);}
std::unique_ptr<FighterObject> YukariCharacter::create_object(){return std::make_unique<YukariObject>();}
void YukariCharacter::advance_object(FighterObject& object,FighterActor& opponent,ReplayStreams& random,FighterActionEvents& events){advance_yukari_object(static_cast<YukariObject&>(object),actor,opponent,state,random,events);}
void YukariCharacter::initialize_story(FighterActor& opponent,StoryEnvironment& environment){initialize_yukari_story(actor,opponent,state.story,environment);}
void YukariCharacter::advance_story(FighterActor& opponent,StoryEnvironment& environment,StoryEvents& events){advance_yukari_story(actor,opponent,state.story,state.introWait,environment,events);}
void YukariCharacter::render_story(u8 stage,ZunGraphics& graphics,const std::array<u32,4>& textures){state.story.render(stage,actor,state.captionWait,graphics,textures);}
void YukariCharacter::choose_ai_input(const FighterActor& opponent,bool story,ReplayStreams& streams,CharacterAIContext& context){ auto& random=streams.random;if(story)choose_yukari_story_input(actor,opponent,state,random,context);else choose_yukari_duel_input(actor,opponent,state,random,context);}
void YukariCharacter::select_commands(const FighterActor& opponent,GameMode mode,ReplayRandom& random){
    struct Callbacks final:FighterActionCallbacks {
        YukariCharacter& character;const FighterActor& opponent;
        Callbacks(YukariCharacter& character,const FighterActor& opponent):character(character),opponent(opponent){}
        void movement_commands(FighterActor& a)override{a.movement.select(a.motion,opponent.motion,a.controls.current,a.animation);}
        void attack_commands(FighterActor& a)override{select_yukari_commands(a,opponent,character.state);}
    } callbacks(*this,opponent);
    actor.guarded_commands(opponent,mode,random,callbacks);
}
}
