#include "MarisaCharacter.hpp"
#include "MarisaObjects.hpp"
#include "MarisaStory.hpp"
#include "MarisaRender.hpp"
namespace th075 {
void MarisaCharacter::reset(FighterCoreEvents& events){reset_marisa(actor,state,events);}
void MarisaCharacter::recognize_inputs(){actor.command=recognize_marisa_command(actor.controls.history,actor.motion.facing);}
void MarisaCharacter::advance_action(FighterActor& opponent,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){advance_marisa_action(actor,opponent,state,mode,random,replayMode,core,events);}
void MarisaCharacter::advance_character_frame(FighterActor& opponent,GameMode mode,ReplayStreams& random,FighterActionEvents& events){advance_marisa_frame(actor,opponent,state.frame,events);}
std::unique_ptr<FighterObject> MarisaCharacter::create_object(){return std::make_unique<MarisaObject>();}
void MarisaCharacter::advance_object(FighterObject& object,FighterActor& opponent,ReplayStreams& random,FighterActionEvents& events){advance_marisa_object(static_cast<MarisaObject&>(object),actor,opponent,state,random,events);}
void MarisaCharacter::initialize_story(FighterActor& opponent,StoryEnvironment& environment){initialize_marisa_story(actor,opponent,state.story,environment);}
void MarisaCharacter::advance_story(FighterActor& opponent,StoryEnvironment& environment,StoryEvents& events){advance_marisa_story(actor,opponent,state.story,state.introWait,environment,events);}
void MarisaCharacter::render_story(u8 stage,ZunGraphics& graphics,const std::array<u32,4>& textures){state.story.render(stage,actor,state.captionWait,graphics,textures);}
void MarisaCharacter::render_special(const CameraMotion& camera,ZunGraphics& graphics,const std::vector<u32>& textures){if(textures.size()>631)render_marisa_laser(actor,state.laser,camera,graphics,textures[631]);}
void MarisaCharacter::choose_ai_input(const FighterActor& opponent,bool story,ReplayStreams& streams,CharacterAIContext& context){ auto& random=streams.random;if(story)choose_marisa_story_input(actor,opponent,state,random,context);else choose_marisa_duel_input(actor,opponent,state,random,context);}
void MarisaCharacter::select_commands(const FighterActor& opponent,GameMode mode,ReplayRandom& random){
    struct Callbacks final:FighterActionCallbacks {
        MarisaCharacter& character;const FighterActor& opponent;
        Callbacks(MarisaCharacter& character,const FighterActor& opponent):character(character),opponent(opponent){}
        void movement_commands(FighterActor& a)override{a.movement.select(a.motion,opponent.motion,a.controls.current,a.animation);}
        void attack_commands(FighterActor& a)override{select_marisa_commands(a,opponent,character.state);}
    } callbacks(*this,opponent);
    actor.guarded_commands(opponent,mode,random,callbacks);
}
}
