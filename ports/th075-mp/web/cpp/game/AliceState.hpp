#pragma once
#include "FighterActionEvents.hpp"
#include "StorySequence.hpp"
#include "CharacterAI.hpp"
#include "ReplayFile.hpp"
namespace th075 {
struct AliceState {StoryCutscene story;i16 actionTicks=0;i16 actionStage=0;i16 volleyTicks=0;i16 volleyCount=0;i16 knockbackX=0;i16 knockbackY=0;i16 dollCenterX=0;i16 dollCenterY=0;i16 dollRadius=0;i16 dollPhase=0;i16 farRangeFrames=0;i16 introWait=0,captionWait=0;i8 difficulty=0;};
void advance_alice_action(FighterActor&,FighterActor&,AliceState&,GameMode,ReplayStreams&,u8,FighterCoreEvents&,FighterActionEvents&);
void reset_alice(FighterActor&,AliceState&,FighterCoreEvents&);
u8 recognize_alice_command(const InputHistory&,i8 facing);
void select_alice_commands(FighterActor&,const FighterActor&,const AliceState&);
void advance_alice_frame(FighterActor&,FighterActor&,AliceState&,FighterActionEvents&);
void choose_alice_duel_input(FighterActor&,const FighterActor&,const AliceState&,ReplayRandom&,CharacterAIContext&);
void choose_alice_story_input(FighterActor&,const FighterActor&,const AliceState&,ReplayRandom&,CharacterAIContext&);
}
