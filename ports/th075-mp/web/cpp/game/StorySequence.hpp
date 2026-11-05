#pragma once
#include "FighterActor.hpp"
#include "ZunGraphics.hpp"
#include "QuadTransform.hpp"
namespace th075 {
// Original story callbacks share stocked lives (completedStages) and
// remaining continues (route) with the battle rules/session. BattleScene binds
// those fields before/after simulation; they are not independent counters.
struct StoryEnvironment {
    u8 stage=1,difficulty=0,background=0,fadeAlpha=0,completedStages=0,route=0;
    u32 fadeColor=0;CameraMotion camera;std::array<u8,2> hudAlpha{255,255};
};
struct StoryCutscene {
    u8 step=0,opponent=0,nextStage=1;i16 phaseTimer=0;
    std::array<float,4> x{},y{};std::array<u8,4> alpha{};
    void render(u8 stage,FighterActor&,i16& captionWait,ZunGraphics&,const std::array<u32,4>& texture);
};
struct StoryEvents {
    virtual ~StoryEvents()=default;
    virtual void begin_dialogue(i16 label)=0;
    virtual void show_dialogue(bool)=0;
    virtual bool advance_dialogue()=0;
    virtual void hide_portrait(FighterActor&)=0;
    virtual void show_portrait(FighterActor&,u8 portrait,u8 direction)=0;
    virtual void advance_portrait(FighterActor&)=0;
    virtual void reload_stage(u8)=0;
    virtual void select_music(i16)=0;
    virtual void start_music()=0;
    virtual void stop_music()=0;
    virtual void sound(i16)=0;
    virtual void replace_text(bool erase)=0;
};
void set_story_phase(FighterActor& opponent,u8 slot,i16 health,u8 kind,u8 card,u8 variant,i32 value,i32 time,i32 drain);
void initialize_reimu_story(FighterActor&,FighterActor&,StoryCutscene&,StoryEnvironment&);
}
