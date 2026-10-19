#pragma once
#include "FighterActionEvents.hpp"
namespace th075 {
struct ReimuRecoveryState {i16 loops=0,impactVelocity=0,smokeEmitted=0,boundaryAngle=0;};
// Returns false for attack/spell actions, which the character attack owner handles.
bool advance_reimu_recovery(FighterActor&,const FighterActor& opponent,ReimuRecoveryState&,GameMode,FighterActionEvents&);
void consume_reimu_spirit(FighterActor&,i16 amount,FighterActionEvents&);
}
