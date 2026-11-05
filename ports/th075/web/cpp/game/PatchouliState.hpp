#pragma once
#include "FighterActionEvents.hpp"
#include "StorySequence.hpp"
#include "CharacterAI.hpp"
#include "ReplayFile.hpp"
namespace th075 {
struct PatchouliState {StoryCutscene story;i16 actionTicks=0;i16 actionStage=0;i16 emissionIndex=0;i16 emissionDelay=0;i16 effectLoops=0;i16 blockEffectSent=0;i16 orbitAngle=0;i16 spinAngle=0;i16 windForceTime=0;i16 farRangeFrames=0;i16 introWait=0,captionWait=0;i8 difficulty=0;};
void advance_patchouli_action(FighterActor&,FighterActor&,PatchouliState&,GameMode,ReplayStreams&,u8,FighterCoreEvents&,FighterActionEvents&);
void reset_patchouli(FighterActor&,PatchouliState&,FighterCoreEvents&);
u8 recognize_patchouli_command(const InputHistory&,i8 facing);
void select_patchouli_commands(FighterActor&,const FighterActor&,PatchouliState&);
void advance_patchouli_frame(FighterActor&,FighterActor&,PatchouliState&,ReplayStreams&,FighterActionEvents&);
void choose_patchouli_duel_input(FighterActor&,const FighterActor&,const PatchouliState&,ReplayRandom&,CharacterAIContext&);
void choose_patchouli_story_input(FighterActor&,const FighterActor&,const PatchouliState&,ReplayRandom&,CharacterAIContext&);
}
