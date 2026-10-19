#pragma once
#include "FighterActionEvents.hpp"
#include "StorySequence.hpp"
#include "CharacterAI.hpp"
#include "ReplayFile.hpp"
namespace th075 {
struct YuyukoState {StoryCutscene story;i16 impactX=0;i16 impactY=0;i16 aimAngle=0;i16 emissionDelay=0;i16 effectLoops=0;i16 guardEffectSent=0;i16 orbitRadius=0;i16 orbitAngle=0;i16 farRangeFrames=0;i16 introWait=0,captionWait=0;i8 difficulty=0;};
void advance_yuyuko_action(FighterActor&,FighterActor&,YuyukoState&,GameMode,ReplayStreams&,u8,FighterCoreEvents&,FighterActionEvents&);
void reset_yuyuko(FighterActor&,YuyukoState&,FighterCoreEvents&);
u8 recognize_yuyuko_command(const InputHistory&,i8 facing);
void select_yuyuko_commands(FighterActor&,const FighterActor&,YuyukoState&);
void advance_yuyuko_frame(FighterActor&,FighterActor&,YuyukoState&,ReplayStreams&,FighterActionEvents&);
void choose_yuyuko_duel_input(FighterActor&,const FighterActor&,const YuyukoState&,ReplayRandom&,CharacterAIContext&);
void choose_yuyuko_story_input(FighterActor&,const FighterActor&,const YuyukoState&,ReplayRandom&,CharacterAIContext&);
}
