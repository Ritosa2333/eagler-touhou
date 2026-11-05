#pragma once
#include "CharacterController.hpp"
namespace th075 {
void update_character_input(CharacterController&,const FighterActor&,const ControlCounts&,GameMode,ReplayStreams&,CharacterAIContext&,u8 difficulty,i32 cancelCount=0);
void restrict_battle_input(FighterActor&,BattlePhase);
}
