#pragma once
#include "FighterActionEvents.hpp"
#include "StorySequence.hpp"
#include "CharacterAI.hpp"
#include "ReplayFile.hpp"
namespace th075 {
struct SuikaState {StoryCutscene story;i16 impactX=0;i16 impactY=0;i16 spellLoops=0;i16 guardEffectSent=0;i16 summonTimer=0;i16 giantMode=0;i16 giantTicks=0;i16 anchorX=0;i16 anchorY=0;i16 farRangeFrames=0;i16 introWait=0,captionWait=0,endWait=0;i8 difficulty=0;};
void advance_suika_action(FighterActor&,FighterActor&,SuikaState&,GameMode,ReplayStreams&,u8,FighterCoreEvents&,FighterActionEvents&);
void reset_suika(FighterActor&,SuikaState&,FighterCoreEvents&);
u8 recognize_suika_command(const InputHistory&,i8 facing);
void select_suika_commands(FighterActor&,const FighterActor&,SuikaState&);
void advance_suika_frame(FighterActor&,FighterActor&,SuikaState&,ReplayStreams&,FighterActionEvents&);
void choose_suika_duel_input(FighterActor&,const FighterActor&,const SuikaState&,ReplayRandom&,CharacterAIContext&);
void choose_suika_story_input(FighterActor&,const FighterActor&,const SuikaState&,ReplayRandom&,CharacterAIContext&);
}
