#pragma once
#include "FighterActionEvents.hpp"
#include "StorySequence.hpp"
#include "CharacterAI.hpp"
#include "ReplayFile.hpp"
namespace th075 {
struct YukariState {StoryCutscene story;i16 impactX=0;i16 impactY=0;i16 emissionIndex=0;i16 emissionDelay=0;i16 effectLoops=0;i16 guardEffectSent=0;i16 portalTimer=0;i16 portalX=0;i16 portalY=0;i16 orbitAngle=0;i16 portalCount=0;i16 portalAngle=0;i16 lastPortalX=0;i16 lastPortalY=0;i16 particleDelay=0,farRangeFrames=0; i16 introWait=0,captionWait=0;i8 difficulty=0;};
void advance_yukari_action(FighterActor&,FighterActor&,YukariState&,GameMode,ReplayStreams&,u8,FighterCoreEvents&,FighterActionEvents&);
void reset_yukari(FighterActor&,YukariState&,FighterCoreEvents&);
u8 recognize_yukari_command(const InputHistory&,i8 facing);
void select_yukari_commands(FighterActor&,const FighterActor&,YukariState&);
void advance_yukari_frame(FighterActor&,FighterActor&,YukariState&,ReplayStreams&,FighterActionEvents&);
void choose_yukari_duel_input(FighterActor&,const FighterActor&,const YukariState&,ReplayRandom&,CharacterAIContext&);
void choose_yukari_story_input(FighterActor&,const FighterActor&,const YukariState&,ReplayRandom&,CharacterAIContext&);
}
