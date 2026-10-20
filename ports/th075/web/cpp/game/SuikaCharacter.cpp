#include "SuikaCharacter.hpp"
#include "SuikaObjects.hpp"
#include "SuikaStory.hpp"
namespace th075 {
void SuikaCharacter::reset(FighterCoreEvents& events){reset_suika(actor,state,events);}
void SuikaCharacter::recognize_inputs(){actor.command=recognize_suika_command(actor.controls.history,actor.motion.facing);}
void SuikaCharacter::advance_action(FighterActor& opponent,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){advance_suika_action(actor,opponent,state,mode,random,replayMode,core,events);}
void SuikaCharacter::advance_character_frame(FighterActor& opponent,GameMode mode,ReplayStreams& random,FighterActionEvents& events){advance_suika_frame(actor,opponent,state,random,events);}
std::unique_ptr<FighterObject> SuikaCharacter::create_object(){return std::make_unique<SuikaObject>();}
void SuikaCharacter::advance_object(FighterObject& object,FighterActor& opponent,ReplayStreams& random,FighterActionEvents& events){advance_suika_object(static_cast<SuikaObject&>(object),actor,opponent,state,random,events);}
void SuikaCharacter::initialize_story(FighterActor& opponent,StoryEnvironment& environment){initialize_suika_story(actor,opponent,state.story,environment);}
void SuikaCharacter::advance_story(FighterActor& opponent,StoryEnvironment& environment,StoryEvents& events){advance_suika_story(actor,opponent,state.story,state.introWait,environment,events);}
void SuikaCharacter::render_story(u8 stage,ZunGraphics& graphics,const std::array<u32,4>& textures){state.story.render(stage,actor,state.endWait,graphics,textures);}
void SuikaCharacter::choose_ai_input(const FighterActor& opponent,bool story,ReplayStreams& streams,CharacterAIContext& context){ auto& random=streams.random;if(story)choose_suika_story_input(actor,opponent,state,random,context);else choose_suika_duel_input(actor,opponent,state,random,context);}
void SuikaCharacter::select_commands(const FighterActor& opponent,GameMode mode,ReplayRandom& random){
    struct Callbacks final:FighterActionCallbacks {
        SuikaCharacter& character;const FighterActor& opponent;
        Callbacks(SuikaCharacter& character,const FighterActor& opponent):character(character),opponent(opponent){}
        void movement_commands(FighterActor& a)override{a.movement.select(a.motion,opponent.motion,a.controls.current,a.animation);}
        void attack_commands(FighterActor& a)override{select_suika_commands(a,opponent,character.state);}
    } callbacks(*this,opponent);
    actor.guarded_commands(opponent,mode,random,callbacks);
}
}
