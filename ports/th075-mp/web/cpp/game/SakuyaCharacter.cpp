#include "SakuyaCharacter.hpp"
#include "SakuyaObjects.hpp"
#include "SakuyaStory.hpp"
namespace th075 {
void SakuyaCharacter::reset(FighterCoreEvents& events){reset_sakuya(actor,state,events);}
void SakuyaCharacter::recognize_inputs(){actor.command=recognize_sakuya_command(actor.controls.history,actor.motion.facing);}
void SakuyaCharacter::advance_action(FighterActor& opponent,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){advance_sakuya_action(actor,opponent,state,mode,random,replayMode,core,events);}
void SakuyaCharacter::advance_character_frame(FighterActor& opponent,GameMode mode,ReplayStreams& random,FighterActionEvents& events){advance_sakuya_frame(actor,opponent,state,events);}
std::unique_ptr<FighterObject> SakuyaCharacter::create_object(){return std::make_unique<SakuyaObject>();}
void SakuyaCharacter::advance_object(FighterObject& object,FighterActor& opponent,ReplayStreams& random,FighterActionEvents& events){advance_sakuya_object(static_cast<SakuyaObject&>(object),actor,opponent,state,random,events);}
void SakuyaCharacter::initialize_story(FighterActor& opponent,StoryEnvironment& environment){initialize_sakuya_story(actor,opponent,state.story,environment);}
void SakuyaCharacter::advance_story(FighterActor& opponent,StoryEnvironment& environment,StoryEvents& events){advance_sakuya_story(actor,opponent,state.story,state.introWait,environment,events);}
void SakuyaCharacter::render_story(u8 stage,ZunGraphics& graphics,const std::array<u32,4>& textures){state.story.render(stage,actor,state.captionWait,graphics,textures);}
void SakuyaCharacter::choose_ai_input(const FighterActor& opponent,bool story,ReplayStreams& streams,CharacterAIContext& context){ auto& random=streams.random;if(story)choose_sakuya_story_input(actor,opponent,state,random,context);else choose_sakuya_duel_input(actor,opponent,state,random,context);}
void SakuyaCharacter::select_commands(const FighterActor& opponent,GameMode mode,ReplayRandom& random){
    struct Callbacks final:FighterActionCallbacks {
        SakuyaCharacter& character;const FighterActor& opponent;
        Callbacks(SakuyaCharacter& character,const FighterActor& opponent):character(character),opponent(opponent){}
        void movement_commands(FighterActor& a)override{a.movement.select(a.motion,opponent.motion,a.controls.current,a.animation);}
        void attack_commands(FighterActor& a)override{select_sakuya_commands(a,opponent,character.state);}
    } callbacks(*this,opponent);
    actor.guarded_commands(opponent,mode,random,callbacks);
}
}
