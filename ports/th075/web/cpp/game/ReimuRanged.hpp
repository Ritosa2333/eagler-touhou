#pragma once
#include "ReimuRecovery.hpp"
#include "ReplayFile.hpp"
namespace th075 {
bool advance_reimu_ranged(FighterActor&,const FighterActor& opponent,ReimuRecoveryState&,ReplayStreams&,FighterActionEvents&);
bool advance_reimu_special_melee(FighterActor&,const FighterActor& opponent,ReimuRecoveryState&,FighterActionEvents&);
}
