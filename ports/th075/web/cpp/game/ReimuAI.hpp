#pragma once
#include "FighterActor.hpp"
#include "CharacterAI.hpp"
namespace th075 {
void choose_reimu_duel_input(FighterActor&,const FighterActor&,ReplayRandom&,CharacterAIContext&);
void choose_reimu_story_input(FighterActor&,const FighterActor&,ReplayRandom&,CharacterAIContext&);
}
