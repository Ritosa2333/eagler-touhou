#pragma once
#include "FighterActionEvents.hpp"
#include "StorySequence.hpp"
#include "CharacterAI.hpp"
#include "ReplayFile.hpp"
namespace th075 {
struct MarisaFrameState {i16 blockEffectSent=0,farRangeFrames=0;};
struct MarisaLaserState {
    bool visible=false;float x=0,y=0,width=0;i8 facing=1;
    std::array<ScreenVertex,74> vertices{};
};
struct MarisaState {
    StoryCutscene story;MarisaFrameState frame;MarisaLaserState laser;
    i16 actionTicks=0,actionStage=0,volleyIndex=0;
    std::array<i16,2> recoveryCounters{};std::array<i16,3> effectCounters{};
    i16 chargeMode=0,chargeCount=0,chargeRemaining=0;
    std::array<i16,13> spellCounters{};
    i16 introWait=0,captionWait=0;i8 difficulty=0;
};
void advance_marisa_action(FighterActor&,FighterActor&,MarisaState&,GameMode,ReplayStreams&,u8 replayMode,FighterCoreEvents&,FighterActionEvents&);
void reset_marisa(FighterActor&,MarisaState&,FighterCoreEvents&);
void advance_marisa_frame(FighterActor&,const FighterActor&,MarisaFrameState&,FighterActionEvents&);
u8 recognize_marisa_command(const InputHistory&,i8 facing);
void select_marisa_commands(FighterActor&,const FighterActor&,const MarisaState&);
void choose_marisa_duel_input(FighterActor&,const FighterActor&,const MarisaState&,ReplayRandom&,CharacterAIContext&);
void choose_marisa_story_input(FighterActor&,const FighterActor&,const MarisaState&,ReplayRandom&,CharacterAIContext&);
}
