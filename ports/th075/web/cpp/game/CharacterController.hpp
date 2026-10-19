#pragma once
#include "FighterActor.hpp"
#include "FighterActionEvents.hpp"
#include "ReplayFile.hpp"
#include "CharacterAI.hpp"
#include "StorySequence.hpp"
#include "OwnedObjects.hpp"
namespace th075 {
struct CharacterController:FighterObjectFactory {
    FighterActor actor;float groundOffset=-10;u16 spellPortraitImage=0;
    std::array<u8,10> victoryRoutes{4,4,4,4,4,4,4,1,1,3};
    virtual ~CharacterController()=default;
    virtual const char* name()const=0;
    virtual void reset(FighterCoreEvents&)=0;
    virtual void set_difficulty(u8)=0;
    virtual void render_special(const CameraMotion&,ZunGraphics&,const std::vector<u32>&){}
    virtual void recognize_inputs()=0;
    virtual void advance_action(FighterActor& opponent,GameMode,ReplayStreams&,u8 replayMode,FighterCoreEvents&,FighterActionEvents&)=0;
    virtual void advance_character_frame(FighterActor& opponent,GameMode mode,ReplayStreams& random,FighterActionEvents&)=0;
    virtual void advance_object(FighterObject&,FighterActor&,ReplayStreams&,FighterActionEvents&)=0;
    virtual void initialize_story(FighterActor&,StoryEnvironment&)=0;
    virtual void advance_story(FighterActor&,StoryEnvironment&,StoryEvents&)=0;
    virtual void render_story(u8 stage,ZunGraphics&,const std::array<u32,4>& textures)=0;
    virtual u8 story_opponent()const=0;virtual u8 next_story_stage()const=0;
    virtual StoryCutscene& story_sequence()=0;
    void prepare_story(u8 stage);
    void set_next_story_stage(u8 stage){story_sequence().nextStage=stage;}
    virtual void choose_ai_input(const FighterActor&,bool story,ReplayStreams&,CharacterAIContext&)=0;
    virtual void select_commands(const FighterActor& opponent,GameMode,ReplayRandom&)=0;
    u8 player_input(const ControlCounts& controls){
        const u8 held=actor.controls.player_frame(controls,actor.motion.facing,actor.animation.cursor.action);
        actor.rawInputs[0]=controls.horizontal;actor.rawInputs[1]=controls.vertical;
        for(u32 i=0;i<4;i++)actor.rawInputs[2+i]=controls.buttons[i];recognize_inputs();return held;
    }
};
}
