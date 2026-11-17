#pragma once
#include "../game/BattleControls.hpp"
namespace th075::sdl::multiplayer {
bool active();
bool pump(double nowUs);
bool step(double nowUs,bool drawForward,bool& advanced);
double pace(double elapsedMs);
void render();
}
