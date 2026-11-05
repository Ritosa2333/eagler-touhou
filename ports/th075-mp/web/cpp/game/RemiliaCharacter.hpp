#pragma once
#include "CharacterController.hpp"
#include "RemiliaState.hpp"
namespace th075 {
class RemiliaCharacter final:public CharacterController {
public:
    RemiliaState state;
    RemiliaCharacter(){spellPortraitImage=672;state.story.nextStage=1;actor.portraitMaximum=11;}
    void set_difficulty(u8 value)override{state.difficulty=i8(value);}
    const char* name()const override{return "remilia";}
    void reset(FighterCoreEvents&)override;
    void recognize_inputs()override;
    void advance_action(FighterActor& opponent,GameMode,ReplayStreams&,u8 replayMode,FighterCoreEvents&,FighterActionEvents&)override;
    void advance_character_frame(FighterActor& opponent,GameMode mode,ReplayStreams& random,FighterActionEvents&)override;
    std::unique_ptr<FighterObject> create_object()override;
    void advance_object(FighterObject&,FighterActor&,ReplayStreams&,FighterActionEvents&)override;
    void initialize_story(FighterActor&,StoryEnvironment&)override;
    void advance_story(FighterActor&,StoryEnvironment&,StoryEvents&)override;
    void render_story(u8 stage,ZunGraphics&,const std::array<u32,4>& textures)override;
    StoryCutscene& story_sequence()override{return state.story;}
    u8 story_opponent()const override{return state.story.opponent;}
    u8 next_story_stage()const override{return state.story.nextStage;}
    void choose_ai_input(const FighterActor&,bool story,ReplayStreams&,CharacterAIContext&)override;
    void select_commands(const FighterActor& opponent,GameMode,ReplayRandom&)override;
};
}
