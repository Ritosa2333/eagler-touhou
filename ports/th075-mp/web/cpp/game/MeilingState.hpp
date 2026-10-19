#pragma once
#include "FighterActionEvents.hpp"
#include "StorySequence.hpp"
#include "CharacterAI.hpp"
#include "ReplayFile.hpp"
namespace th075 {
struct MeilingState {StoryCutscene story;i16 impactX=0;i16 impactY=0;i16 emissionIndex=0;i16 emissionDelay=0;i16 effectLoops=0;i16 guardEffectSent=0;i16 spellLoops=0;i16 spellTimer=0;i16 spellAngle=0;i16 chargeFrames=0;i16 frameCounter=0;i16 farRangeFrames=0;i16 introWait=0,captionWait=0;i8 difficulty=0;};
void advance_meiling_action(FighterActor&,FighterActor&,MeilingState&,GameMode,ReplayStreams&,u8,FighterCoreEvents&,FighterActionEvents&);
void reset_meiling(FighterActor&,MeilingState&,FighterCoreEvents&);
u8 recognize_meiling_command(const InputHistory&,i8 facing);
void select_meiling_commands(FighterActor&,const FighterActor&,MeilingState&);
void advance_meiling_frame(FighterActor&,FighterActor&,MeilingState&,ReplayStreams&,FighterActionEvents&);
void choose_meiling_duel_input(FighterActor&,const FighterActor&,const MeilingState&,ReplayRandom&,CharacterAIContext&);
void choose_meiling_story_input(FighterActor&,const FighterActor&,const MeilingState&,ReplayRandom&,CharacterAIContext&);
}
