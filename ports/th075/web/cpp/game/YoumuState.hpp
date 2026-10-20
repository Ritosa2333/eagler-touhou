#pragma once
#include "FighterActionEvents.hpp"
#include "StorySequence.hpp"
#include "CharacterAI.hpp"
#include "ReplayFile.hpp"
#include <deque>
namespace th075 {
struct PhantomPose {i16 action=0,index=0;float x=0,y=0;i8 facing=1;i16 hits=0;};

struct YoumuState {StoryCutscene story;std::deque<PhantomPose> history;i16 impactX=0;i16 impactY=0;i16 emissionIndex=0;i16 emissionDelay=0;i16 effectLoops=0;i16 summonWait=0;i16 phantomActivated=0;i16 phantomSpawned=0;i16 phantomTime=0;i16 phantomAttacking=0;i16 farRangeFrames=0;i16 introWait=0,captionWait=0;i8 difficulty=0;};
void advance_youmu_action(FighterActor&,FighterActor&,YoumuState&,GameMode,ReplayStreams&,u8,FighterCoreEvents&,FighterActionEvents&);
void reset_youmu(FighterActor&,YoumuState&,FighterCoreEvents&);
u8 recognize_youmu_command(const InputHistory&,i8 facing);
void select_youmu_commands(FighterActor&,const FighterActor&,YoumuState&);
void advance_youmu_frame(FighterActor&,FighterActor&,YoumuState&,GameMode,ReplayStreams&,FighterActionEvents&);
void choose_youmu_duel_input(FighterActor&,const FighterActor&,const YoumuState&,ReplayStreams&,CharacterAIContext&);
void choose_youmu_story_input(FighterActor&,const FighterActor&,const YoumuState&,ReplayStreams&,CharacterAIContext&);
}
