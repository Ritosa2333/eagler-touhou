#pragma once
#include "../game/BattleControls.hpp"
namespace th075::sdl::multiplayer {
bool active();
bool pump(double nowUs);
bool step(const BattleSystemInput&,double nowUs,bool* advanced=nullptr);
double pace(double elapsedMs);
void render();
}
