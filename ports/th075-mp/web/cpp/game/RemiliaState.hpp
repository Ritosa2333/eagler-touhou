#pragma once
#include "FighterActionEvents.hpp"
#include "StorySequence.hpp"
#include "CharacterAI.hpp"
#include "ReplayFile.hpp"
namespace th075 {
struct RemiliaState {StoryCutscene story;i16 impactX=0;i16 impactY=0;i16 aimAngle=0;i16 emissionDelay=0;i16 effectLoops=0;i16 guardEffectSent=0;i16 farRangeFrames=0;i16 introWait=0,captionWait=0;i8 difficulty=0;};
void advance_remilia_action(FighterActor&,FighterActor&,RemiliaState&,GameMode,ReplayStreams&,u8,FighterCoreEvents&,FighterActionEvents&);
void reset_remilia(FighterActor&,RemiliaState&,FighterCoreEvents&);
u8 recognize_remilia_command(const InputHistory&,i8 facing);
void select_remilia_commands(FighterActor&,const FighterActor&,RemiliaState&);
void advance_remilia_frame(FighterActor&,FighterActor&,RemiliaState&,ReplayStreams&,FighterActionEvents&);
void choose_remilia_duel_input(FighterActor&,const FighterActor&,const RemiliaState&,ReplayRandom&,CharacterAIContext&);
void choose_remilia_story_input(FighterActor&,const FighterActor&,const RemiliaState&,ReplayRandom&,CharacterAIContext&);
}
