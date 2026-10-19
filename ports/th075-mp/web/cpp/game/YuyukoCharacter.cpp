#include "YuyukoCharacter.hpp"
#include "YuyukoObjects.hpp"
#include "YuyukoStory.hpp"
namespace th075 {
void YuyukoCharacter::reset(FighterCoreEvents& events){reset_yuyuko(actor,state,events);}
void YuyukoCharacter::recognize_inputs(){actor.command=recognize_yuyuko_command(actor.controls.history,actor.motion.facing);}
void YuyukoCharacter::advance_action(FighterActor& opponent,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){advance_yuyuko_action(actor,opponent,state,mode,random,replayMode,core,events);}
void YuyukoCharacter::advance_character_frame(FighterActor& opponent,GameMode mode,ReplayStreams& random,FighterActionEvents& events){advance_yuyuko_frame(actor,opponent,state,random,events);}
std::unique_ptr<FighterObject> YuyukoCharacter::create_object(){return std::make_unique<YuyukoObject>();}
void YuyukoCharacter::advance_object(FighterObject& object,FighterActor& opponent,ReplayStreams& random,FighterActionEvents& events){advance_yuyuko_object(static_cast<YuyukoObject&>(object),actor,opponent,state,random,events);}
void YuyukoCharacter::initialize_story(FighterActor& opponent,StoryEnvironment& environment){initialize_yuyuko_story(actor,opponent,state.story,environment);}
void YuyukoCharacter::advance_story(FighterActor& opponent,StoryEnvironment& environment,StoryEvents& events){advance_yuyuko_story(actor,opponent,state.story,state.introWait,environment,events);}
void YuyukoCharacter::render_story(u8 stage,ZunGraphics& graphics,const std::array<u32,4>& textures){state.story.render(stage,actor,state.captionWait,graphics,textures);}
void YuyukoCharacter::choose_ai_input(const FighterActor& opponent,bool story,ReplayStreams& streams,CharacterAIContext& context){ auto& random=streams.random;if(story)choose_yuyuko_story_input(actor,opponent,state,random,context);else choose_yuyuko_duel_input(actor,opponent,state,random,context);}
void YuyukoCharacter::select_commands(const FighterActor& opponent,GameMode mode,ReplayRandom& random){
    struct Callbacks final:FighterActionCallbacks {
        YuyukoCharacter& character;const FighterActor& opponent;
        Callbacks(YuyukoCharacter& character,const FighterActor& opponent):character(character),opponent(opponent){}
        void movement_commands(FighterActor& a)override{a.movement.select(a.motion,opponent.motion,a.controls.current,a.animation);}
        void attack_commands(FighterActor& a)override{select_yuyuko_commands(a,opponent,character.state);}
    } callbacks(*this,opponent);
    actor.guarded_commands(opponent,mode,random,callbacks);
}
}
