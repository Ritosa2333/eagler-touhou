#pragma once
#include "FighterActionEvents.hpp"
#include "StorySequence.hpp"
#include "CharacterAI.hpp"
#include "ReplayFile.hpp"
namespace th075 {
struct SakuyaState {StoryCutscene story;i16 actionTicks=0;i16 actionStage=0;i16 emissionDelay=0;i16 volleyLoops=0;i16 burstCount=0;i16 blockEffectSent=0;i16 clockReserve=0;i16 clockPulse=0;i16 timeReserve=0;i16 timeActive=0;i16 knivesActive=0;i16 farRangeFrames=0;i16 introWait=0,captionWait=0;i8 difficulty=0;};
void advance_sakuya_action(FighterActor&,FighterActor&,SakuyaState&,GameMode,ReplayStreams&,u8,FighterCoreEvents&,FighterActionEvents&);
void reset_sakuya(FighterActor&,SakuyaState&,FighterCoreEvents&);
u8 recognize_sakuya_command(const InputHistory&,i8 facing);
void select_sakuya_commands(FighterActor&,const FighterActor&,const SakuyaState&);
void advance_sakuya_frame(FighterActor&,FighterActor&,SakuyaState&,FighterActionEvents&);
void choose_sakuya_duel_input(FighterActor&,const FighterActor&,const SakuyaState&,ReplayRandom&,CharacterAIContext&);
void choose_sakuya_story_input(FighterActor&,const FighterActor&,const SakuyaState&,ReplayRandom&,CharacterAIContext&);
}
