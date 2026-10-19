#include "PatchouliCharacter.hpp"
#include "PatchouliObjects.hpp"
#include "PatchouliStory.hpp"
namespace th075 {
void PatchouliCharacter::reset(FighterCoreEvents& events){reset_patchouli(actor,state,events);}
void PatchouliCharacter::recognize_inputs(){actor.command=recognize_patchouli_command(actor.controls.history,actor.motion.facing);}
void PatchouliCharacter::advance_action(FighterActor& opponent,GameMode mode,ReplayStreams& random,u8 replayMode,FighterCoreEvents& core,FighterActionEvents& events){advance_patchouli_action(actor,opponent,state,mode,random,replayMode,core,events);}
void PatchouliCharacter::advance_character_frame(FighterActor& opponent,GameMode mode,ReplayStreams& random,FighterActionEvents& events){advance_patchouli_frame(actor,opponent,state,random,events);}
std::unique_ptr<FighterObject> PatchouliCharacter::create_object(){return std::make_unique<PatchouliObject>();}
void PatchouliCharacter::advance_object(FighterObject& object,FighterActor& opponent,ReplayStreams& random,FighterActionEvents& events){advance_patchouli_object(static_cast<PatchouliObject&>(object),actor,opponent,state,random,events);}
void PatchouliCharacter::initialize_story(FighterActor& opponent,StoryEnvironment& environment){initialize_patchouli_story(actor,opponent,state.story,environment);}
void PatchouliCharacter::advance_story(FighterActor& opponent,StoryEnvironment& environment,StoryEvents& events){advance_patchouli_story(actor,opponent,state.story,state.introWait,environment,events);}
void PatchouliCharacter::render_story(u8 stage,ZunGraphics& graphics,const std::array<u32,4>& textures){state.story.render(stage,actor,state.captionWait,graphics,textures);}
void PatchouliCharacter::choose_ai_input(const FighterActor& opponent,bool story,ReplayStreams& streams,CharacterAIContext& context){ auto& random=streams.random;if(story)choose_patchouli_story_input(actor,opponent,state,random,context);else choose_patchouli_duel_input(actor,opponent,state,random,context);}
void PatchouliCharacter::select_commands(const FighterActor& opponent,GameMode mode,ReplayRandom& random){
    struct Callbacks final:FighterActionCallbacks {
        PatchouliCharacter& character;const FighterActor& opponent;
        Callbacks(PatchouliCharacter& character,const FighterActor& opponent):character(character),opponent(opponent){}
        void movement_commands(FighterActor& a)override{a.movement.select(a.motion,opponent.motion,a.controls.current,a.animation);}
        void attack_commands(FighterActor& a)override{select_patchouli_commands(a,opponent,character.state);}
    } callbacks(*this,opponent);
    actor.guarded_commands(opponent,mode,random,callbacks);
}
}
